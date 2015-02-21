/********************************************************************
 * livegrep -- svn_indexer.h
 * Copyright (c) 2011-2013 Nelson Elhage
 *
 * This program is free software. You may use, redistribute, and/or
 * modify it under the terms listed in the COPYING file.
 ********************************************************************/
#ifndef CODESEARCH_SVN_INDEXER_H
#define CODESEARCH_SVN_INDEXER_H

#include <string>
#include "svncpp/client.hpp"

class code_searcher;

namespace svn
{
  class Client;
  class Revision;
};

struct indexed_tree;
struct json_object;

class svn_indexer {
public:
    svn_indexer(code_searcher *cs,
                const std::string& name,
                json_object *metadata = 0);
    ~svn_indexer();
    void walk(std::string ref);
protected:
    void walk_dir(std::string pfx);

    code_searcher *cs_;
    svn::Client *client_;
    const indexed_tree *idx_tree_;
    std::string name_;
    json_object *metadata_;
    svn::Revision rev_;
    int  len_base_url_;
};

#endif
