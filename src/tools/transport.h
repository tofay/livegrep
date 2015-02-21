/********************************************************************
 * livegrep -- tools/transport.h
 * Copyright (c) 2011-2014 Nelson Elhage
 *
 * This program is free software. You may use, redistribute, and/or
 * modify it under the terms listed in the COPYING file.
 ********************************************************************/
#ifndef CODESEARCH_TRANSPORT_H
#define CODESEARCH_TRANSPORT_H

#include <sys/time.h>
#include <string>
#include <vector>
#include <stdio.h>

struct match_result;
struct match_stats;
struct json_object;
struct index_info;
struct query;

struct repo_spec {
    std::string path;
    std::string name;
    std::vector<std::string> revisions;
    json_object *metadata;

    repo_spec() : metadata(NULL) {}
};

struct index_spec {
    std::string name;
    std::vector<std::string> paths;
    std::vector<std::string> svn_urls;
    std::vector<repo_spec> repos;
};

class codesearch_transport {
public:
    codesearch_transport(FILE *in, FILE *out);
    void write_match(const match_result *m);
    void write_error(const std::string &err);
    void write_ready(const index_info *info);
    void write_done(timeval, const match_stats *);
    bool read_query(query *, bool *done);
protected:
    void write_frame(const std::string &opcode, json_object *body);
    FILE *in_, *out_;
};

struct json_parse_error {
    json_parse_error() : ok_(true) {}
    json_parse_error(const std::string &err) : ok_(false), error(err) {}

    bool ok() {
        return ok_;
    }

    std::string err() {
        if (path.size()) {
            return path + ": " + error;
        } else {
            return error;
        }
    }

    json_parse_error wrap(std::string path) {
        json_parse_error wrapped = *this;
        if (wrapped.path.size()) {
            wrapped.path = path + "." + wrapped.path;
        } else {
            wrapped.path = path;
        }
        return wrapped;
    }
private:
    bool ok_;
    std::string error;
    std::string path;
};

json_parse_error parse_index_spec(json_object *in, index_spec *out);

#endif
