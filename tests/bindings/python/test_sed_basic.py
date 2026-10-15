# Copyright libOpenCOR contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import libopencor as loc
import os
import platform
import pytest
import shutil
import tempfile
import utils
from utils import assert_issues


def sedml_contents(model_source, with_simulation=False):
    simulations = (
        """    <listOfSimulations>
        <uniformTimeCourse id="simulation" initialTime="0" outputStartTime="0" outputEndTime="50" numberOfSteps="50">
            <algorithm kisaoID="KISAO:0000019"/>
        </uniformTimeCourse>
    </listOfSimulations>
"""
        if with_simulation
        else ""
    )

    return f"""<?xml version='1.0' encoding='UTF-8'?>
<sedML level="1" version="3" xmlns="http://sed-ml.org/sed-ml/level1/version3">
    <listOfModels>
        <model id="model" language="urn:sedml:language:cellml" source="{model_source}"/>
    </listOfModels>
{simulations}</sedML>
"""


def test_no_file():
    document = loc.SedDocument()

    assert not document.has_issues

    document = loc.SedDocument(None)

    assert not document.has_issues


def test_unknown_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an unknown file.",
        ],
    ]

    file = loc.File(utils.resource_path("unknown_file.txt"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_cellml_file():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)

    assert not document.has_issues


def test_sedml_file():
    file = loc.File(utils.resource_path("cellml_2.sedml"))
    document = loc.SedDocument(file)

    assert document.has_issues

    needed_file = loc.File(utils.resource_path("cellml_2.cellml"))

    document = loc.SedDocument(file)

    assert not document.has_issues


def test_sedml_file_with_absolute_cellml_file():
    file = loc.File(utils.resource_path("api/sed/absolute_cellml_file.sedml"))
    document = loc.SedDocument(file)

    assert document.has_issues

    needed_file = loc.File(utils.resource_path("file.txt"))

    document = loc.SedDocument(file)

    assert not document.has_issues


def test_sedml_file_with_relative_cellml_file():
    # A relative model source is relative to the SED-ML file and its leading ".." components must be kept.

    file = loc.File(utils.resource_path("api/sed/relative_cellml_file.sedml"), False)

    file.contents = utils.text_to_list(sedml_contents("../../cellml_2.cellml"))

    document = loc.SedDocument(file)

    assert len(document.models) == 1
    assert document.models[0].file.path == utils.resource_path("cellml_2.cellml")

    needed_file = loc.File(utils.resource_path("cellml_2.cellml"))

    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_sedml_file_with_relative_cellml_file_in_working_directory():
    # A relative model source is relative to the SED-ML file, not to the current working directory, even if the
    # current working directory contains a file with that name.

    orig_dir = os.getcwd()

    os.chdir(utils.resource_path())

    file = loc.File(utils.resource_path("api/sed/relative_cellml_file.sedml"), False)

    file.contents = utils.text_to_list(sedml_contents("cellml_2.cellml"))

    document = loc.SedDocument(file)

    os.chdir(orig_dir)

    assert len(document.models) == 1
    assert document.models[0].file.path == utils.resource_path(
        "api/sed/cellml_2.cellml"
    )


@pytest.mark.skipif(
    platform.system() == "Windows",
    reason="A symbolic link usually requires some special privileges on Windows.",
)
def test_sedml_file_with_relative_cellml_file_through_symbolic_link():
    # A relative model source that starts with a ".." component is relative to the actual location of the SED-ML file,
    # i.e. to the parent directory of the target of a symbolic link if the SED-ML file is accessed through it (e.g.,
    # "link/../model.cellml" refers to "real/model.cellml" if "link" is a symbolic link to "real/sub").
    # Note: a symbolic link usually requires some special privileges on Windows.

    dir_path = os.path.join(
        tempfile.gettempdir(),
        "libopencor_sedml_file_with_relative_cellml_file_through_symbolic_link",
    )

    shutil.rmtree(dir_path, ignore_errors=True)
    os.makedirs(os.path.join(dir_path, "real", "sub"))
    os.makedirs(os.path.join(dir_path, "cwd"))
    shutil.copyfile(
        utils.resource_path("cellml_2.cellml"),
        os.path.join(dir_path, "real", "model.cellml"),
    )
    os.symlink("../real/sub", os.path.join(dir_path, "cwd", "link"))

    orig_dir = os.getcwd()

    os.chdir(os.path.join(dir_path, "cwd"))

    file = loc.File("link/simulation.sedml", False)

    file.contents = utils.text_to_list(sedml_contents("../model.cellml"))

    document = loc.SedDocument(file)

    os.chdir(orig_dir)

    assert len(document.models) == 1
    assert document.models[0].file.path == "link/../model.cellml"
    assert document.models[0].file.type == loc.File.Type.CellmlFile

    shutil.rmtree(dir_path)


def test_sedml_file_with_unsupported_scheme_cellml_file():
    # A model source with a scheme other than "http", "https", or "file" is an absolute URI that we cannot retrieve, so
    # its model is ignored rather than considered to be relative to the SED-ML file. No task is then created for the
    # simulation since it would have no model.

    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the model 'urn:miriam:biomodels.db:BIOMD0000000012' has an unsupported scheme. It has been ignored.",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/unsupported_scheme_cellml_file.sedml"), False
    )

    file.contents = utils.text_to_list(
        sedml_contents("urn:miriam:biomodels.db:BIOMD0000000012", True)
    )

    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
    assert len(document.models) == 0
    assert len(document.simulations) == 1
    assert len(document.tasks) == 0


def test_sedml_file_with_empty_source_cellml_file():
    # An empty model source refers to the SED-ML file itself (see https://www.rfc-editor.org/rfc/rfc3986#section-5.2.2),
    # but libSEDML considers it to be an error, so the file is not considered to be a SED-ML file (and its model source
    # never gets resolved).

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an unknown file.",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/empty_source_cellml_file.sedml"), False
    )

    file.contents = utils.text_to_list(sedml_contents(""))

    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
    assert len(document.models) == 0


def test_sedml_file_with_file_url_cellml_file():
    # A "file" URL model source refers to a local file if it has no host (e.g., "file:///some/path/model.cellml" or
    # "file:/some/path/model.cellml") or the "localhost" host (e.g., "file://localhost/some/path/model.cellml"), and to
    # a remote file otherwise (e.g., "file://example.com/some/path/model.cellml").

    if platform.system() == "Windows":
        url_path = "/" + utils.forward_slash_path(
            utils.resource_path("cellml_2.cellml")
        )
    else:
        url_path = utils.resource_path("cellml_2.cellml")

    needed_file = loc.File(utils.resource_path("cellml_2.cellml"))

    for source in [
        "file://" + url_path,
        "file:" + url_path,
        "file://localhost" + url_path,
    ]:
        file = loc.File(
            utils.resource_path("api/sed/file_url_cellml_file.sedml"), False
        )

        file.contents = utils.text_to_list(sedml_contents(source))

        document = loc.SedDocument(file)

        assert not document.has_issues
        assert len(document.models) == 1
        assert document.models[0].file == needed_file

    file = loc.File(utils.resource_path("api/sed/file_url_cellml_file.sedml"), False)

    file.contents = utils.text_to_list(sedml_contents("file://example.com" + url_path))

    needed_remote_file = loc.File("file://example.com" + url_path, False)
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_remote_file


def test_sedml_file_with_query_and_fragment_cellml_file():
    # The query and fragment of a model source are ignored unless they are percent-encoded, in which case they are part
    # of the file name of the model.

    file = loc.File(
        utils.resource_path("api/sed/query_and_fragment_cellml_file.sedml"), False
    )

    file.contents = utils.text_to_list(sedml_contents("../../cellml_2.cellml?a=b#c"))

    needed_file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file

    encoded_file = loc.File(
        utils.resource_path("api/sed/encoded_query_and_fragment_cellml_file.sedml"),
        False,
    )

    encoded_file.contents = utils.text_to_list(
        sedml_contents("../../cellml_2.cellml%3Fa=b%23c")
    )

    encoded_needed_file = loc.File(utils.resource_path("cellml_2.cellml?a=b#c"), False)
    encoded_document = loc.SedDocument(encoded_file)

    assert not encoded_document.has_issues
    assert len(encoded_document.models) == 1
    assert encoded_document.models[0].file == encoded_needed_file


def test_sedml_file_with_query_only_cellml_file():
    # A query-only model source refers to the SED-ML file itself since the query of a model source is ignored.

    file = loc.File(utils.resource_path("api/sed/query_only_cellml_file.sedml"), False)

    file.contents = utils.text_to_list(sedml_contents("?model=1"))

    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == file


def test_sedml_file_with_fragment_only_cellml_file():
    # A fragment-only model source refers to the SED-ML file itself since the fragment of a model source is ignored.

    file = loc.File(
        utils.resource_path("api/sed/fragment_only_cellml_file.sedml"), False
    )

    file.contents = utils.text_to_list(sedml_contents("#model"))

    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == file


def test_remote_sedml_file_with_relative_cellml_file():
    # A relative model source is relative to the remote SED-ML file, whatever the platform and even if the query and/or
    # fragment of the SED-ML file's URL contain some forward slashes.

    file = loc.File("https://example.com/simulations/simulation.sedml?a=b/c#d/e", False)

    file.contents = utils.text_to_list(sedml_contents("../models/./model.cellml"))

    needed_file = loc.File("https://example.com/models/model.cellml", False)
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_remote_sedml_file_without_path_with_relative_cellml_file():
    # A relative model source is relative to the root of a remote SED-ML file which URL has no path.

    file = loc.File("https://example.com?a=b/c", False)

    file.contents = utils.text_to_list(sedml_contents("model.cellml"))

    needed_file = loc.File("https://example.com/model.cellml", False)
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_remote_sedml_file_with_absolute_cellml_file():
    # An absolute model source is relative to the origin of the remote SED-ML file, i.e. never a local file name.

    file = loc.File("https://example.com/simulations/simulation.sedml?a=b/c#d/e", False)

    file.contents = utils.text_to_list(sedml_contents("/models/./model.cellml"))

    needed_file = loc.File("https://example.com/models/model.cellml", False)
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_remote_sedml_file_with_network_path_cellml_file():
    # A network-path model source is relative to the scheme of the remote SED-ML file, i.e. never a local file name.

    file = loc.File("https://example.com/simulations/simulation.sedml", False)

    file.contents = utils.text_to_list(
        sedml_contents("//example.org/models/model.cellml")
    )

    needed_file = loc.File("https://example.org/models/model.cellml", False)
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_remote_sedml_file_with_query_only_cellml_file():
    # A query-only model source is relative to the URL of the remote SED-ML file without its query and fragment, i.e.
    # never a local file name.

    file = loc.File("https://example.com/simulations/simulation.sedml?a=b/c#d/e", False)

    file.contents = utils.text_to_list(sedml_contents("?model=1"))

    needed_file = loc.File(
        "https://example.com/simulations/simulation.sedml?model=1", False
    )
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_remote_sedml_file_with_fragment_only_cellml_file():
    # A fragment-only model source is relative to the URL of the remote SED-ML file without its fragment, i.e. never a
    # local file name.

    file = loc.File("https://example.com/simulations/simulation.sedml?a=b/c#d/e", False)

    file.contents = utils.text_to_list(sedml_contents("#model"))

    needed_file = loc.File(
        "https://example.com/simulations/simulation.sedml?a=b/c#model", False
    )
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_remote_sedml_file_with_remote_cellml_file():
    # A model source that is a URL is used as is (its scheme being normalised), i.e. it is not relative to the remote
    # SED-ML file.

    file = loc.File("https://example.com/simulations/simulation.sedml", False)

    file.contents = utils.text_to_list(
        sedml_contents("HTTPS://example.org/models/model.cellml")
    )

    needed_file = loc.File("https://example.org/models/model.cellml", False)
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_remote_sedml_file_with_unsupported_scheme_cellml_file():
    # A model source with a scheme other than "http", "https", or "file" is an absolute URI that we cannot retrieve, so
    # its model is ignored rather than considered to be relative to the remote SED-ML file.

    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the model 'urn:miriam:biomodels.db:BIOMD0000000012' has an unsupported scheme. It has been ignored.",
        ],
    ]

    file = loc.File("https://example.com/simulations/simulation.sedml", False)

    file.contents = utils.text_to_list(
        sedml_contents("urn:miriam:biomodels.db:BIOMD0000000012")
    )

    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
    assert len(document.models) == 0


def test_remote_sedml_file_with_empty_source_cellml_file():
    # An empty model source refers to the remote SED-ML file itself (see
    # https://www.rfc-editor.org/rfc/rfc3986#section-5.2.2), but libSEDML considers it to be an error, so the file is
    # not considered to be a SED-ML file (and its model source never gets resolved).

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an unknown file.",
        ],
    ]

    file = loc.File("https://example.com/simulations/simulation.sedml", False)

    file.contents = utils.text_to_list(sedml_contents(""))

    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
    assert len(document.models) == 0


def test_sedml_file_with_remote_cellml_file():
    file = loc.File(utils.resource_path("api/sed/remote_cellml_file.sedml"))
    document = loc.SedDocument(file)

    assert document.has_issues

    needed_file = loc.File(utils.RemoteFile)

    document = loc.SedDocument(file)

    assert not document.has_issues


def test_combine_archive():
    file = loc.File(utils.resource_path("cellml_2.omex"))
    document = loc.SedDocument(file)

    assert not document.has_issues


def test_combine_archive_with_no_manifest_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/no_manifest_file.omex"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_one_cellml_file():
    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_one_cellml_file.omex")
    )
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert document.simulations[0].output_end_time == 1000.0

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues


def test_combine_archive_with_no_master_file_and_one_unknown_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_one_unknown_cellml_file.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_one_unknown_https_cellml_file():
    # The format of an unknown file is used to determine its type, and a format may use the "https" scheme (e.g.,
    # "https://identifiers.org/combine.specifications/cellml.1.1").

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(
        utils.resource_path(
            "api/sed/no_master_file_with_one_unknown_https_cellml_file.omex"
        )
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_several_cellml_files():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file, no SED-ML file, and several CellML files.",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_several_cellml_files.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_master_file_with_duplicate_location():
    # The master attribute of an entry that is ignored because its location refers to the same file as another entry
    # still applies to that file.

    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "COMBINE archive: the file 'cellml_2_b.cellml' is the same as the file './cellml_2_b.cellml'. It has been ignored.",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/master_file_with_duplicate_location.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(file, expected_issues)
    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == file.child_file("./cellml_2_b.cellml")


def test_combine_archive_with_no_master_file_and_one_sedml_file():
    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_one_sedml_file.omex")
    )
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert document.simulations[0].output_end_time == 50.0

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues


def test_combine_archive_with_no_master_file_and_one_unknown_sedml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_one_unknown_sedml_file.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_one_unknown_versioned_sedml_file():
    # The format of an unknown file is used to determine its type, and a format may be versioned and use the "https"
    # scheme (e.g., "https://identifiers.org/combine.specifications/sed-ml.level-1.version-3"). So, the unknown SED-ML
    # file is the master file, rather than the CellML file.

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(
        utils.resource_path(
            "api/sed/no_master_file_with_one_unknown_versioned_sedml_file.omex"
        )
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_several_sedml_files():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file and several SED-ML files.",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_several_sedml_files.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_unknown_direct_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unknown_direct_cellml_file.omex"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_unknown_indirect_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task: task 'task1' requires a model of CellML type.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unknown_indirect_cellml_file.omex"))
    document = loc.SedDocument(file)

    assert not document.has_issues

    instance = document.instantiate()

    instance.run()

    assert_issues(instance, expected_issues)


def test_combine_archive_with_unknown_sedml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unknown_sedml_file.omex"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_irretrievable_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an irretrievable file.",
        ],
    ]

    file = loc.File(utils.resource_path("irretrievable_file.txt"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
