//#include <gflags/gflags.h>
#include <string>

#include "codesearch.h"
#include "metrics.h"
#include "svn_indexer.h"
#include "debug.h"
#include "svncpp/dirent.hpp"

namespace {
    metric svn_walk("timer.svn.walk");
    metric svn_contents("timer.svn.contents");
};

svn_indexer::svn_indexer(code_searcher *cs,
                         const std::string& name,
                         json_object *metadata)
    : cs_(cs), name_(name), metadata_(metadata) {
    svn::Context * context = new svn::Context();
    client_ = new svn::Client(context);
    rev_ = svn::Revision::HEAD;
}

svn_indexer::~svn_indexer() {
}

void svn_indexer::walk(std::string url) {
    idx_tree_ = cs_->open_tree(name_, metadata_, "");
    len_base_url_ = url.length();
    walk_dir(url);
}

void svn_indexer::walk_dir(std::string url) {

    std::cout << "Walking " + url << std::endl;

    metric::timer tm_walk(svn_walk);
    svn::DirEntries dirEntries;
    try{
        dirEntries = client_->list(url.c_str(), rev_, false);
    }
    catch (svn::ClientException & e) {
        std::cout << e.message() << std::endl;
        return;
    }
    tm_walk.pause();
    svn::DirEntries::const_iterator it;
    for (it = dirEntries.begin(); it != dirEntries.end(); it++) {
        const svn::DirEntry & dirEntry = *it;
        std::string path = url + "/" + std::string(dirEntry.name());
        if (dirEntry.kind() == svn_node_dir)        {
            walk_dir(path);
        }
        else {
            cs_->index_file(idx_tree_,
                            path.substr(len_base_url_),
                            client_->cat(path, rev_, rev_));
        }
    }
    tm_walk.start();
}
