/*
 * COPYRIGHT (C) 2017-2019, zhllxt
 *
 * author   : zhllxt
 * email    : 37792738@qq.com
 * 
 * Distributed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * (See accompanying file LICENSE or see <http://www.gnu.org/licenses/>)
 *
 * KCP MTU , UDP PACKET MAX LENGTH 576
 */

#ifndef __ZEPHYR_XD_KCP_UTIL_HPP__
#define __ZEPHYR_XD_KCP_UTIL_HPP__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <memory>
#include <future>
#include <utility>
#include <string_view>
#include <chrono>

#include <zephyr/base/selector.hpp>
#include <zephyr/base/error.hpp>
#include <zephyr/base/detail/condition_wrap.hpp>

#include <zephyr/util/ikcp.h>
#include <zephyr/util/packet.hpp>

namespace zephyr::detail::xd_kcp {

#if defined(__GNUC__) || defined(__GNUG__)
#define __KCPHDR_ONEBYTE_ALIGN__ __attribute__((packed))
#elif defined(_MSC_VER)
#define __KCPHDR_ONEBYTE_ALIGN__
#pragma pack(push, 1)
#endif

enum class xd_kcp_protocal_type : uint8_t
{
    NONE = 0,
    SYN, // connect
    ACK, // connected
    FIN, // disconnect
    MSG, // msg
};

struct xd_kcphdr_syn
{
    std::uint8_t protocal_type = static_cast<uint8_t>(xd_kcp_protocal_type::SYN); // syn
    std::uint32_t local_id;                                                       // client_id
} __KCPHDR_ONEBYTE_ALIGN__;

struct xd_kcphdr_ack
{
    std::uint8_t protocal_type = static_cast<uint8_t>(xd_kcp_protocal_type::ACK); // ack
    std::uint32_t local_id;                                                       // server_id
    std::uint32_t remote_id;                                                      // client_id
} __KCPHDR_ONEBYTE_ALIGN__;

struct xd_kcphdr_fin
{
    std::uint8_t protocal_type = static_cast<uint8_t>(xd_kcp_protocal_type::FIN);
    std::uint32_t local_id;
    std::uint32_t remote_id;
    std::uint32_t error_code;
} __KCPHDR_ONEBYTE_ALIGN__;

struct xd_kcphdr_msg
{
    std::uint8_t protocal_type = static_cast<uint8_t>(xd_kcp_protocal_type::MSG); // msg
    std::uint32_t local_id;                                                       // client_id
} __KCPHDR_ONEBYTE_ALIGN__;

#if defined(__GNUC__) || defined(__GNUG__)
#undef __KCPHDR_ONEBYTE_ALIGN__
#elif defined(_MSC_VER)
#pragma pack(pop)
#undef __KCPHDR_ONEBYTE_ALIGN__
#endif

template<typename = void>
inline bool is_xd_kcphdr_syn(std::string_view s)
{
    if (s.size() != sizeof(xd_kcphdr_syn))
    {
        return false;
    }

    xd_kcphdr_syn* hdr = (xd_kcphdr_syn*)(s.data());
    return hdr->protocal_type == static_cast<std::uint8_t>(xd_kcp_protocal_type::SYN);
}

template<typename = void>
inline bool is_xd_kcphdr_synack(std::string_view s, std::uint32_t local_id)
{
    if (s.size() != sizeof(xd_kcphdr_ack))
    {
        return false;
    }

    xd_kcphdr_ack* hdr = (xd_kcphdr_ack*)(s.data());
    // TODO: how to get remote_id
    return (hdr->protocal_type == static_cast<std::uint8_t>(xd_kcp_protocal_type::ACK)) && (hdr->local_id == local_id);
}

template<typename = void>
inline bool is_xd_kcphdr_fin(std::string_view s, std::uint32_t local_id)
{
    if (s.size() != sizeof(xd_kcphdr_fin))
    {
        return false;
    }

    xd_kcphdr_fin* hdr = (xd_kcphdr_fin*)(s.data());
    return (hdr->protocal_type == static_cast<std::uint8_t>(xd_kcp_protocal_type::FIN)) && (hdr->local_id == local_id);
}

template<typename = void>
inline bool is_xd_kcphdr_msg(std::string_view s, std::uint32_t local_id)
{
    // 1 flag + 4 local_id + 4 kcp_conv_id
    if (s.size() < sizeof(xd_kcphdr_msg) + sizeof(std::uint32_t))
    {
        return false;
    }

    zephyr::detail::BasePacketReader reader(s.data(), sizeof(xd_kcphdr_msg));
    xd_kcphdr_msg hdr;
    hdr.protocal_type = reader.readUINT8();
    hdr.local_id = reader.readUINT32();
    return (hdr.protocal_type == static_cast<std::uint8_t>(xd_kcp_protocal_type::MSG)) && (hdr.local_id == local_id);
}

template<typename = void>
inline xd_kcphdr_syn xd_make_kcphdr_syn(std::uint32_t local_id)
{
    xd_kcphdr_syn hdr;
    hdr.local_id = local_id;

    return hdr;
}

template<typename = void>
inline xd_kcphdr_ack xd_make_kcphdr_synack(std::uint32_t local_id, std::uint32_t remote_id)
{
    xd_kcphdr_ack hdr;
    hdr.local_id = local_id;
    hdr.remote_id = remote_id;

    return hdr;
}

template<typename = void>
inline xd_kcphdr_fin xd_make_kcphdr_fin(std::uint32_t local_id, std::uint32_t remote_id, std::uint32_t error_code)
{
    xd_kcphdr_fin hdr;
    hdr.local_id = local_id;
    hdr.remote_id = remote_id;
    hdr.error_code = error_code;

    return hdr;
}

template<typename = void>
inline xd_kcphdr_msg xd_make_kcphdr_msg(std::uint32_t local_id)
{
    xd_kcphdr_msg hdr;
    hdr.local_id = local_id;

    return hdr;
}

struct kcp_deleter
{
    inline void operator()(kcp::ikcpcb* p) const
    {
        kcp::ikcp_release(p);
    };
};

} // namespace zephyr::detail::xd_kcp

#endif // !__ZEPHYR_XD_KCP_UTIL_HPP__
