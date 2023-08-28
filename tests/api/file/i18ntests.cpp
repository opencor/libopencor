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

#include "gtest/gtest.h"

#include "tests/utils.h"

#include <libopencor>

TEST(BasicFileTest, arabicFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/ملف/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/ملف/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/ملف/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(BasicFileTest, chineseFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/文件夹/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/文件夹/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/文件夹/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(BasicFileTest, hindiFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/फोल्डर/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/फोल्डर/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/फोल्डर/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(BasicFileTest, japaneseFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/フォルダ/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/フォルダ/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/フォルダ/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(BasicFileTest, russianFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/папка/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/папка/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/папка/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}
