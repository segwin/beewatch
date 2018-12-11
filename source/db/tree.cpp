//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "db/tree.h"

#include "util/logging.h"
#include "util/string.h"

#include <algorithm>
#include <sstream>

namespace beewatch::db
{

    //==============================================================================
    Node::Node(Ptr parent, std::string value, std::vector<Ptr> children)
        : _parent(parent), _value(value), _children(children)
    {
    }

    Node::Ptr Node::create(Ptr parent, std::string value, std::vector<Ptr> children)
    {
        return std::make_shared<Node>(parent, value, children);
    }

    Node::Ptr Node::get()
    {
        return shared_from_this();
    }

    //==============================================================================
    void Node::addChild(Ptr child)
    {
        std::unique_lock<std::shared_mutex> lockChild(child->_mutex);

        // Remove node from previous parent, if any
        auto prevParent = child->_parent;

        if (prevParent)
        {
            std::unique_lock<std::shared_mutex> lockPrevParent(prevParent->_mutex);

            child->_parent->removeChild(child);
            child->_parent = nullptr;
        }

        // Add child to this node
        std::unique_lock<std::shared_mutex> lockNode(_mutex);

        child->_parent = get();
        _children.push_back(child);
    }

    void Node::removeChild(Ptr child)
    {
        std::unique_lock<std::shared_mutex> lockNode(_mutex);
        std::unique_lock<std::shared_mutex> lockChild(child->_mutex);

        auto it = std::find_if(_children.begin(), _children.end(), child);

        if (it == _children.end())
        {
            lockChild.unlock();
            g_logger.print(Logger::Error, "Failed to find child node: " + child->str());
            return;
        }

        _children.erase(it);
    }

    //==============================================================================
    std::string Node::str() const
    {
        std::ostringstream oss;

        oss << _value << " {" << std::endl;

        for (auto child : _children)
            oss << string::indent(child->str(), 2) << std::endl;

        oss << "}" << std::endl;

        return oss.str();
    }

} // namespace beewatch::db
