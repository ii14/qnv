#pragma once

#include "msgpack_view/view.hpp"
#include <array>
#include <tuple>
#include <utility>
#include <vector>
#include "qnv/ApiTypes.hpp"

namespace msgpack_view {

template <> struct convert<bool> {
    static bool as(object_view view) {
        if (view.ref.type == msgpack::type::BOOLEAN)
            return view.ref.via.boolean;
        throw msgpack_view::type_error { };
    }
};

template <> struct convert<uint64_t> {
    static uint64_t as(object_view view) {
        if (view.ref.type == msgpack::type::POSITIVE_INTEGER)
            return view.ref.via.u64;
        throw msgpack_view::type_error { };
    }
};

template <> struct convert<int64_t> {
    static int64_t as(object_view view) {
        if (view.ref.type == msgpack::type::POSITIVE_INTEGER)
            return view.ref.via.u64;
        if (view.ref.type == msgpack::type::NEGATIVE_INTEGER)
            return view.ref.via.i64;
        throw msgpack_view::type_error { };
    }
};

template <> struct convert<double> {
    static double as(object_view view) {
        if (view.ref.type == msgpack::type::FLOAT64)
            return view.ref.via.f64;
        if (view.ref.type == msgpack::type::FLOAT32) // is this even needed?
            return view.ref.via.f64;
        throw msgpack_view::type_error { };
    }
};

template <> struct convert<array_view> {
    static array_view as(object_view view) {
        if (view.ref.type == msgpack::type::ARRAY)
            return { view.ref.via.array };
        throw msgpack_view::type_error { };
    }
};

template <> struct convert<map_view> {
    static map_view as(object_view view) {
        if (view.ref.type == msgpack::type::MAP)
            return { view.ref.via.map };
        throw msgpack_view::type_error { };
    }
};

template <> struct convert<std::string_view> {
    static std::string_view as(object_view view) {
        if (view.ref.type == msgpack::type::STR)
            return { view.ref.via.str.ptr, view.ref.via.str.size };
        throw msgpack_view::type_error { };
    }
};

template <> struct convert<std::string> {
    static std::string as(object_view view) {
        if (view.ref.type == msgpack::type::STR)
            return { view.ref.via.str.ptr, view.ref.via.str.size };
        throw msgpack_view::type_error { };
    }
};

template <> struct convert<Buffer> {
    static Buffer as(object_view view) {
        if (view.ref.type != msgpack::type::EXT)
            throw msgpack_view::type_error { };
        if (view.ref.via.ext.type() != Buffer::kMsgpackType)
            throw msgpack_view::type_error { };
        auto d = msgpack::unpack(view.ref.via.ext.data(), view.ref.via.ext.size);
        return Buffer { object_view { d.get() }.as<int64_t>() };
    }
};

template <> struct convert<Window> {
    static Window as(object_view view) {
        if (view.ref.type != msgpack::type::EXT)
            throw msgpack_view::type_error { };
        if (view.ref.via.ext.type() != Window::kMsgpackType)
            throw msgpack_view::type_error { };
        auto d = msgpack::unpack(view.ref.via.ext.data(), view.ref.via.ext.size);
        return Window { object_view { d.get() }.as<int64_t>() };
    }
};

template <> struct convert<Tabpage> {
    static Tabpage as(object_view view) {
        if (view.ref.type != msgpack::type::EXT)
            throw msgpack_view::type_error { };
        if (view.ref.via.ext.type() != Tabpage::kMsgpackType)
            throw msgpack_view::type_error { };
        auto d = msgpack::unpack(view.ref.via.ext.data(), view.ref.via.ext.size);
        return Tabpage { object_view { d.get() }.as<int64_t>() };
    }
};

template <typename T, typename U> struct convert<std::pair<T, U>> {
    static std::pair<T, U> as(object_view view) {
        auto arr = view.as<array_view>();
        if (arr.size() < 2)
            throw msgpack_view::type_error { };
        return { arr[0].as<T>(), arr[1].as<U>() };
    }
};

template <typename T> struct convert<std::vector<T>> {
    static std::vector<T> as(object_view view) {
        auto arr = view.as<array_view>();
        std::vector<T> res;
        res.reserve(arr.size());
        for (auto item : arr)
            res.push_back(item.as<T>());
        return res;
    }
};

template <typename T, size_t N> struct convert<std::array<T, N>> {
    static std::array<T, N> as(object_view view) {
        auto arr = view.as<array_view>();
        if (arr.size() < N)
            throw msgpack_view::type_error { };
        std::array<T, N> res;
        for (size_t i = 0; i < N; ++i)
            res[i] = arr[i].as<T>();
        return res;
    }
};

// template <typename... T> struct convert<std::tuple<T...>> {
//     using value_type = std::tuple<T...>;
//     static value_type as(object_view view) {
//         auto arr = view.as<array_view>();
//         if (arr.size() < std::tuple_size_v<value_type>)
//             throw msgpack_view::type_error { };
//         // TODO: fold expression
//     }
// };

} // namespace msgpack_view
