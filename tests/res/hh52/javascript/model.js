// The content of this file was generated using the Python profile of libCellML 0.4.0.
// Note: converted from Python to JavaScript using https://thepythoncode.com/assistant/code-converter/javascript/.

const { Enum } = require("enum");
const { pow } = require("mathjs");

const __version__ = "0.4.0";
const LIBCELLML_VERSION = "0.4.0";

const STATE_COUNT = 4;
const VARIABLE_COUNT = 18;

const VariableType = new Enum([
  "VARIABLE_OF_INTEGRATION",
  "STATE",
  "CONSTANT",
  "COMPUTED_CONSTANT",
  "ALGEBRAIC",
]);

const VOI_INFO = {
  name: "time",
  units: "millisecond",
  component: "environment",
  type: VariableType.VARIABLE_OF_INTEGRATION,
};

const STATE_INFO = [
  {
    name: "V",
    units: "millivolt",
    component: "membrane",
    type: VariableType.STATE,
  },
  {
    name: "h",
    units: "dimensionless",
    component: "sodium_channel_h_gate",
    type: VariableType.STATE,
  },
  {
    name: "m",
    units: "dimensionless",
    component: "sodium_channel_m_gate",
    type: VariableType.STATE,
  },
  {
    name: "n",
    units: "dimensionless",
    component: "potassium_channel_n_gate",
    type: VariableType.STATE,
  },
];

const VARIABLE_INFO = [
  {
    name: "i_Stim",
    units: "microA_per_cm2",
    component: "membrane",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "i_L",
    units: "microA_per_cm2",
    component: "leakage_current",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "i_K",
    units: "microA_per_cm2",
    component: "potassium_channel",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "i_Na",
    units: "microA_per_cm2",
    component: "sodium_channel",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "Cm",
    units: "microF_per_cm2",
    component: "membrane",
    type: VariableType.CONSTANT,
  },
  {
    name: "E_R",
    units: "millivolt",
    component: "membrane",
    type: VariableType.CONSTANT,
  },
  {
    name: "E_L",
    units: "millivolt",
    component: "leakage_current",
    type: VariableType.COMPUTED_CONSTANT,
  },
  {
    name: "g_L",
    units: "milliS_per_cm2",
    component: "leakage_current",
    type: VariableType.CONSTANT,
  },
  {
    name: "E_Na",
    units: "millivolt",
    component: "sodium_channel",
    type: VariableType.COMPUTED_CONSTANT,
  },
  {
    name: "g_Na",
    units: "milliS_per_cm2",
    component: "sodium_channel",
    type: VariableType.CONSTANT,
  },
  {
    name: "alpha_m",
    units: "per_millisecond",
    component: "sodium_channel_m_gate",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "beta_m",
    units: "per_millisecond",
    component: "sodium_channel_m_gate",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "alpha_h",
    units: "per_millisecond",
    component: "sodium_channel_h_gate",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "beta_h",
    units: "per_millisecond",
    component: "sodium_channel_h_gate",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "E_K",
    units: "millivolt",
    component: "potassium_channel",
    type: VariableType.COMPUTED_CONSTANT,
  },
  {
    name: "g_K",
    units: "milliS_per_cm2",
    component: "potassium_channel",
    type: VariableType.CONSTANT,
  },
  {
    name: "alpha_n",
    units: "per_millisecond",
    component: "potassium_channel_n_gate",
    type: VariableType.ALGEBRAIC,
  },
  {
    name: "beta_n",
    units: "per_millisecond",
    component: "potassium_channel_n_gate",
    type: VariableType.ALGEBRAIC,
  },
];

function create_states_array() {
  return Array(STATE_COUNT).fill(NaN);
}

function create_variables_array() {
  return Array(VARIABLE_COUNT).fill(NaN);
}

function initialise_variables(states, rates, variables) {
  variables[4] = 1.0;
  variables[5] = 0.0;
  variables[7] = 0.3;
  variables[9] = 120.0;
  variables[15] = 36.0;
  states[0] = 0.0;
  states[1] = 0.6;
  states[2] = 0.05;
  states[3] = 0.325;
}

function compute_computed_constants(variables) {
  variables[6] = variables[5] - 10.613;
  variables[8] = variables[5] - 115.0;
  variables[14] = variables[5] + 12.0;
}

function computeRates(voi, states, rates, variables) {
  variables[0] = voi >= 10.0 && voi <= 10.5 ? -20.0 : 0.0;
  variables[1] = variables[7] * (states[0] - variables[6]);
  variables[2] =
    variables[15] * Math.pow(states[3], 4.0) * (states[0] - variables[14]);
  variables[3] =
    variables[9] *
    Math.pow(states[2], 3.0) *
    states[1] *
    (states[0] - variables[8]);
  rates[0] =
    -(-variables[0] + variables[3] + variables[2] + variables[1]) /
    variables[4];
  variables[10] =
    (0.1 * (states[0] + 25.0)) / (Math.exp((states[0] + 25.0) / 10.0) - 1.0);
  variables[11] = 4.0 * Math.exp(states[0] / 18.0);
  rates[2] = variables[10] * (1.0 - states[2]) - variables[11] * states[2];
  variables[12] = 0.07 * Math.exp(states[0] / 20.0);
  variables[13] = 1.0 / (Math.exp((states[0] + 30.0) / 10.0) + 1.0);
  rates[1] = variables[12] * (1.0 - states[1]) - variables[13] * states[1];
  variables[16] =
    (0.01 * (states[0] + 10.0)) / (Math.exp((states[0] + 10.0) / 10.0) - 1.0);
  variables[17] = 0.125 * Math.exp(states[0] / 80.0);
  rates[3] = variables[16] * (1.0 - states[3]) - variables[17] * states[3];
}

function computeVariables(voi, states, rates, variables) {
  variables[1] = variables[7] * (states[0] - variables[6]);
  variables[3] =
    variables[9] *
    Math.pow(states[2], 3.0) *
    states[1] *
    (states[0] - variables[8]);
  variables[10] =
    (0.1 * (states[0] + 25.0)) / (Math.exp((states[0] + 25.0) / 10.0) - 1.0);
  variables[11] = 4.0 * Math.exp(states[0] / 18.0);
  variables[12] = 0.07 * Math.exp(states[0] / 20.0);
  variables[13] = 1.0 / (Math.exp((states[0] + 30.0) / 10.0) + 1.0);
  variables[2] =
    variables[15] * Math.pow(states[3], 4.0) * (states[0] - variables[14]);
  variables[16] =
    (0.01 * (states[0] + 10.0)) / (Math.exp((states[0] + 10.0) / 10.0) - 1.0);
  variables[17] = 0.125 * Math.exp(states[0] / 80.0);
}
