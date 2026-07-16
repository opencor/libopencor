/*
Copyright libOpenCOR contributors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

import { readFileSync, writeFileSync } from 'node:fs';
import { basename } from 'node:path';

// Find the matching closing brace for a given opening brace.

function findMatchingClosingBrace(text, start) {
  let depth = 0;

  for (let i = start; i < text.length; ++i) {
    if (text[i] === '{') {
      ++depth;
    } else if (text[i] === '}') {
      if (--depth === 0) {
        return i + 1;
      }
    }
  }

  return -1;
}

// Normalise an interface declaration by adding 2 extra spaces of indentation to every non-empty line in the body
// and re-indenting the closing brace by 2 spaces.

function normaliseInterfaceDeclaration(interfaceDeclaration) {
  const openingBraceIndex = interfaceDeclaration.indexOf('{');
  const rest = interfaceDeclaration.slice(openingBraceIndex + 1);
  const closingBraceIndex = rest.lastIndexOf('}');

  return `${interfaceDeclaration.slice(0, openingBraceIndex + 1)}${rest.slice(0, closingBraceIndex).replace(/^(?!$)/gm, '  ')}  ${rest.slice(closingBraceIndex)}`;
}

// Collect dotted interface and type declarations into a map of namespace -> name ->
// { interfaceDeclaration, typeDeclaration }.

const enumValueRegEx = /export interface (\w+\.\w+)Value<T extends number>\s*\{/g;
const filePath = process.argv[2];
let content = readFileSync(filePath, 'utf8');
const namespaceData = new Map();

for (let match = enumValueRegEx.exec(content); match !== null; match = enumValueRegEx.exec(content)) {
  const fullName = match[1];
  const dotIndex = fullName.indexOf('.');
  const namespace = fullName.slice(0, dotIndex);

  if (!namespaceData.has(namespace)) {
    namespaceData.set(namespace, new Map());
  }

  const members = namespaceData.get(namespace);
  const memberName = fullName.slice(dotIndex + 1);
  const baseName = memberName.endsWith('Value') ? memberName.slice(0, -5) : memberName;

  if (!members.has(baseName)) {
    members.set(baseName, {});
  }

  const closingBraceIndex = findMatchingClosingBrace(content, content.indexOf('{', match.index));

  members.get(baseName).interfaceDeclaration = content.slice(match.index, closingBraceIndex);
}

// Collect dotted type declarations into the same map.

const enumTypeRegEx = /export type (\w+\.\w+) = ([\w.]+Value<\d+>(?:\|[\w.]+Value<\d+>)*);/g;

for (let match = enumTypeRegEx.exec(content); match !== null; match = enumTypeRegEx.exec(content)) {
  const fullName = match[1];
  const dotIndex = fullName.indexOf('.');
  const namespace = fullName.slice(0, dotIndex);

  if (!namespaceData.has(namespace)) {
    namespaceData.set(namespace, new Map());
  }

  const members = namespaceData.get(namespace);
  const name = fullName.slice(dotIndex + 1);

  if (!members.has(name)) {
    members.set(name, {});
  }

  members.get(name).typeDeclaration = match[0];
}

// Generate namespace blocks for each namespace and remove the old dotted declarations from the content.

const namespaceBlocks = [];

for (const [namespace, members] of namespaceData) {
  const lines = [`export namespace ${namespace} {`];

  for (const [_name, data] of members) {
    // Remove the namespace prefix from the interface and type declarations.

    const namespaceReferenceRegEx = new RegExp(`${namespace}\\.`, 'g');

    if (data.interfaceDeclaration) {
      const stripped = data.interfaceDeclaration.replace(namespaceReferenceRegEx, '');

      lines.push(`  ${normaliseInterfaceDeclaration(stripped)}`);

      // Remove the old dotted declaration from content.

      content = content.replace(data.interfaceDeclaration, '');
    }

    // Add the type declaration (e.g., export type Type = TypeValue<1> | TypeValue<2>;).

    if (data.typeDeclaration) {
      const stripped = data.typeDeclaration.replace(namespaceReferenceRegEx, '').replace(/\|/g, ' | ');

      lines.push(`  ${stripped}`);

      // Remove the old dotted declaration from content.

      content = content.replace(data.typeDeclaration, '');
    }
  }

  lines.push('}');

  namespaceBlocks.push(lines.join('\n'));
}

// Remove any remaining dotted property declarations from the EmbindModule interface.

for (const [namespace] of namespaceData) {
  const propertyRegEx = new RegExp(`\\n  ${namespace}\\.\\w+: \\{[^}]*\\};`, 'g');

  content = content.replace(propertyRegEx, '');
}

// Remove any excessive blank lines that may have been introduced by the above operations.

content = content.replace(/\n{4,}/g, '\n\n\n');

// Add any missing properties to the EmbindModule interface.

for (const [namespace, members] of namespaceData) {
  const entryRegEx = new RegExp(`(\\n  ${namespace}: \\{[^}]*)(\\};)`);

  if (entryRegEx.test(content)) {
    const existingEntry = content.match(entryRegEx)[1];
    let newProperties = '';

    for (const [name] of members) {
      const propertyLine = `    readonly ${name}: typeof ${namespace}.${name};`;

      if (!existingEntry.includes(propertyLine)) {
        newProperties += `\n${propertyLine}`;
      }
    }

    if (newProperties) {
      content = content.replace(entryRegEx, `${existingEntry.replace(/\s+$/, '')}${newProperties}\n  $2`);
    }
  }
}

// Insert the namespace blocks into the EmbindModule interface.

if (namespaceBlocks.length) {
  const embindIndex = content.search(/\ninterface EmbindModule/);

  if (embindIndex !== -1) {
    const insertPos = embindIndex + 1;

    content = `${content.slice(0, insertPos)}${namespaceBlocks.join('\n\n')}\n\n${content.slice(insertPos)}`;
  }
}

// Add a toString() method to the Issue interface if it doesn't already exist.

const issueInterfaceStart = content.search(/export interface Issue\s+(extends\s+\w+\s+)?\{/);
const issueInterfaceEnd = findMatchingClosingBrace(
  content,
  issueInterfaceStart + content.slice(issueInterfaceStart).indexOf('{')
);

const issueBody = content.slice(issueInterfaceStart, issueInterfaceEnd);

if (!issueBody.includes('toString():')) {
  content = `${content.slice(0, issueInterfaceEnd - 1)}  toString(): string;\n${content.slice(issueInterfaceEnd - 1)}`;
}

// Update the file in place.

writeFileSync(filePath, content, 'utf8');

console.log(`-- Patched ${basename(filePath)}`);
