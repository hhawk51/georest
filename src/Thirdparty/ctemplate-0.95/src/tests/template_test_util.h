// Copyright (c) 2007, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ---
// Author: Jim Morrison

#ifndef TEMPLATE_TEMPLATE_TEST_UTIL_H_
#define TEMPLATE_TEMPLATE_TEST_UTIL_H_

#include "config_for_unittests.h"
#include <string>
#include <vector>
#include HASH_MAP_H           // defined in config.h
#include <ctemplate/template_namelist.h>
#include <ctemplate/template_dictionary_interface.h>
#include <ctemplate/template_dictionary.h>
#include <ctemplate/template_string.h>

_START_GOOGLE_NAMESPACE_

class TemplateDictionary;

inline TemplateId GlobalIdForTest(const char* ptr, int len) {
  return TemplateString(ptr, len).GetGlobalId();
}

// Call this to create a StaticTemplateString for testing when the ptr is
// not guaranteed to be allocated for the entire length of the test.
#define STS_INIT_FOR_TEST(ptr, len, arena) \
  { { arena->Memdup(ptr, len), len, GOOGLE_NAMESPACE::GlobalIdForTest(ptr, len) } };

class TemporaryRegisterTemplate {
 public:
  explicit TemporaryRegisterTemplate(const char* name);
  ~TemporaryRegisterTemplate();
 private:
  TemplateNamelist::NameListType* old_namelist_;
  TemplateNamelist::NameListType namelist_;

  // disallow copy constructor and assignment
  TemporaryRegisterTemplate(const TemporaryRegisterTemplate&);
  void operator=(const TemporaryRegisterTemplate&);
};

// For friendship reasons, we make this a top-level class rather
// than a nested class.  It's used only in TemplateDictionaryPeer.
// We take ownership of the iterator passed to us.  To make sure that
// isn't a problem, we make this class not-copyable.
class TemplateDictionaryPeerIterator {
 public:
  explicit TemplateDictionaryPeerIterator(
      TemplateDictionaryInterface::Iterator* it) : it_(it) { }
  ~TemplateDictionaryPeerIterator() { delete it_; }
  bool HasNext() const { return it_->HasNext(); }
  const TemplateDictionaryInterface& Next() { return it_->Next(); }
 private:
  TemplateDictionaryInterface::Iterator* it_;
  TemplateDictionaryPeerIterator(const TemplateDictionaryPeerIterator&);
  TemplateDictionaryPeerIterator& operator=(
      const TemplateDictionaryPeerIterator&);
};

// This class is meant for use in unittests.  This class wraps the
// TemplateDictionary and provides access to internal data that should
// not be used in production code.  If you need this kind of
// functionality in production, use TemplateDictionaryWrapper or
// TemplateDictionaryInterface; see top of file for details.
//
// Example Usage:
//  TemplateDictionary dict("test dictionary");
//  FillDictionaryValues(&dict);
//
//  TemplateDictionaryPeer peer(&dict);
//  EXPECT_EQ("5", peer.GetSectionValue("width"));
class TemplateDictionaryPeer {
 public:
  explicit TemplateDictionaryPeer(const TemplateDictionary* dict)
      : dict_(dict) {}

  // Returns the value for the named variable.
  const char* GetSectionValue(const TemplateString& variable) const;

  // Returns true if the named section is hidden.
  bool IsHiddenSection(const TemplateString& name) const;

  // Returns true if the named sub-template is hidden.
  bool IsHiddenTemplate(const TemplateString& name) const;

  // Retrieves TemplateDictionary instances for the given section name.  The
  // caller does not assume ownership of the returned TemplateDictionary
  // instances.  The number of instances is returned.  All prior entries in
  // the dicts vector are cleared.
  //
  // NOTE: This method assumes that old-style template dictionaries are not in
  // use.  That is, it assumes that all section dictionaries have been added
  // with AddSectionDictionary rather than AddOldstyleSectionDictionary.
  int GetSectionDictionaries(const TemplateString& section_name,
                             std::vector<const TemplateDictionary*>* dicts) const;

  // Retrieves included TemplateDictionary instances for the given name.  The
  // caller does not assume ownership of the returned TemplateDictionary
  // instances.  The number of instances is returned.  All prior entries in
  // the dicts vector are cleared.
  //
  // NOTE: This method assumes that old-style template dictionaries are not in
  // use.  That is, it assumes that all section dictionaries have been added
  // with AddIncludeDictionary rather than AddOldstyleIncludeDictionary.
  int GetIncludeDictionaries(const TemplateString& section_name,
                             std::vector<const TemplateDictionary*>* dicts) const;

  const char* GetIncludeTemplateName(const TemplateString& variable,
                                     int dictnum) const;

  typedef TemplateDictionaryPeerIterator Iterator;

  Iterator* CreateTemplateIterator(const TemplateString& section)
      const {
    return new Iterator(dict_->CreateTemplateIterator(section));
  }

  Iterator* CreateSectionIterator(const TemplateString& section)
      const {
    return new Iterator(dict_->CreateSectionIterator(section));
  }

  // Returns the filename associated with the TemplateDictionary.
  const char* GetFilename() const;

 private:
  const TemplateDictionary* dict_;  // Not owned.

  // disallow copy constructor and assignment
  TemplateDictionaryPeer(const TemplateDictionaryPeer&);
  void operator=(const TemplateDictionaryPeer&);
};

_END_GOOGLE_NAMESPACE_

#endif  // TEMPLATE_TEMPLATE_TEST_UTIL_H_
