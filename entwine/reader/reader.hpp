/******************************************************************************
* Copyright (c) 2016, Connor Manning (connor@hobu.co)
*
* Entwine -- Point cloud indexing
*
* Entwine is available under the terms of the LGPL2 license. See COPYING
* for specific license text and more information.
*
******************************************************************************/

#pragma once

#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <entwine/reader/query.hpp>
#include <entwine/tree/hierarchy.hpp>
#include <entwine/types/outer-scope.hpp>
#include <entwine/types/structure.hpp>
#include <entwine/third/arbiter/arbiter.hpp>

namespace entwine
{

class InvalidQuery : public std::runtime_error
{
public:
    InvalidQuery()
        : std::runtime_error("Invalid query")
    { }

    InvalidQuery(std::string what)
        : std::runtime_error("Invalid query - " + what)
    { }
};

class BaseChunkReader;
class Bounds;
class Cache;
class Hierarchy;
class Metadata;
class Schema;

class Reader
{
public:
    // Will throw if entwine's meta files cannot be fetched from this endpoint.
    Reader(const arbiter::Endpoint& endpoint, Cache& cache);
    Reader(std::string path, Cache& cache);
    ~Reader();

    template<typename... Args>
    std::vector<char> query(Args&&... args)
    {
        auto q(getQuery(std::forward<Args>(args)...));
        return q->run();
    }

    std::unique_ptr<Query> getQuery(
            std::size_t depth,
            const Point* scale = nullptr,
            const Point* offset = nullptr);

    std::unique_ptr<Query> getQuery(
            const Bounds& qbox,
            std::size_t depth,
            const Point* scale = nullptr,
            const Point* offset = nullptr);

    std::unique_ptr<Query> getQuery(
            std::size_t depthBegin,
            std::size_t depthEnd,
            const Point* scale = nullptr,
            const Point* offset = nullptr);

    std::unique_ptr<Query> getQuery(
            const Bounds& qbox,
            std::size_t depthBegin,
            std::size_t depthEnd,
            const Point* scale = nullptr,
            const Point* offset = nullptr);

    std::unique_ptr<Query> getQuery(
            const Schema& schema,
            const Json::Value& filter,
            std::size_t depthBegin,
            std::size_t depthEnd,
            const Point* scale = nullptr,
            const Point* offset = nullptr);

    std::unique_ptr<Query> getQuery(
            const Schema& schema,
            const Json::Value& filter,
            const Bounds& qbox,
            std::size_t depthBegin,
            std::size_t depthEnd,
            const Point* scale = nullptr,
            const Point* offset = nullptr);

    Json::Value hierarchy(
            const Bounds& qbox,
            std::size_t depthBegin,
            std::size_t depthEnd,
            bool vertical = false,
            const Point* scale = nullptr,
            const Point* offset = nullptr);

    const Metadata& metadata() const { return *m_metadata; }
    std::string path() const { return m_endpoint.root(); }

    const BaseChunkReader* base() const { return m_base.get(); }
    const arbiter::Endpoint& endpoint() const { return m_endpoint; }
    bool exists(const Id& id) const { return m_ids.count(id); }
    const std::set<Id>& ids() const { return m_ids; }

private:
    Bounds localize(
            const Bounds& inBounds,
            const Delta& localDelta) const;

    arbiter::Endpoint m_endpoint;

    std::unique_ptr<Metadata> m_metadata;
    std::unique_ptr<Hierarchy> m_hierarchy;
    std::unique_ptr<BaseChunkReader> m_base;

    Cache& m_cache;
    std::set<Id> m_ids;
};

} // namespace entwine

