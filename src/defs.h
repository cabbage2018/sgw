/*!
 * @file     common.h
 * @brief    Common module interface header file.
 *
 * This module contains common type definitions used in various routines.
 *
 * @copyright Copyright (C) 2020 FeiG(SI China MAC).\n
 *            All rights reserved.
 */

#ifndef FIEAG_COMMON_H_
#define FIEAG_COMMON_H_

// #include "mcl_core/mcl_config_setup.h"

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#ifdef  __cplusplus
extern "C"
{
#endif

typedef size_t fieag_size_t;
/*
typedef int8_t mcl_int8_t;
typedef int16_t mcl_int16_t;
typedef int32_t mcl_int32_t;
typedef int64_t mcl_int64_t;
typedef uint8_t mcl_uint8_t;
typedef uint16_t mcl_uint16_t;
typedef uint32_t mcl_uint32_t;
typedef uint64_t mcl_uint64_t;
typedef mcl_uint8_t mcl_bool_t;
typedef time_t mcl_time_t;
typedef mcl_int32_t mcl_error_t;
*/
#define NUM_THREADS 2
#define ADDRESS "tcp://localhost:1883" //更改此处地址
#define CLIENTID "aaabbbccc_pub" //更改此处客户端ID
#define SUB_CLIENTID "aaabbbccc_sub" //更改此处客户端ID
#define TOPIC "system/sub/entity/edge/gateway/001" //更改发送的话题
#define PAYLOAD "Hello Man, Can you see me ?!" //
#define QOS 1
#define TIMEOUT 10000L
#define USERNAME "test_user"
#define PASSWORD "jim777"
#define DISCONNECT "out"

/**
 * MCL function leave label.
 */
#define FIEAG_FUNCTION_LEAVE_LABEL mcl_function_leave_label

/**
 * MCL bool definition for boolean value of false.
 */
#define FIEAG_FALSE ((mcl_bool_t) 0)

/**
 * MCL bool definition for boolean value of true.
 */
#define FIEAG_TRUE ((mcl_bool_t) 1)

// Maximum value mcl_size_t can have.
// The expression below is portable and safe.
#define FIEAG_SIZE_MAX ((mcl_size_t) - 1)

// FIEAG_NULL Definition. Refer to C-FAQ Q5.2 - Q5.17 (http://c-faq.com/null/safermacs.html).
#define FIEAG_NULL ((void *) 0)

#define FIEAG_NULL_CHAR '\0'
#define FIEAG_NULL_CHAR_SIZE 1

/**
 * Http request payload size limit for MindSphere.
 * */
#define FIEAG_MAXIMUM_HTTP_PAYLOAD_SIZE (10485760)


/**
 * MCL core return code definitions. Every function in core component returning a status code uses this enum values.
 */
typedef enum E_FIEAG_RETURN_CODE
{
    // General return codes.
    FIEAG_OK = 0,                             //!< Success.
    FIEAG_FAIL,                               //!< Internal failure in MCL.
    FIEAG_TRIGGERED_WITH_NULL,                //!< Received parameter is null.
    FIEAG_OUT_OF_MEMORY,                      //!< Memory allocation fail.
    FIEAG_INVALID_PARAMETER,                  //!< General invalid parameter fail.
    FIEAG_INVALID_LOG_LEVEL,                  //!< Given log level is invalid.
    FIEAG_NO_SERVER_TIME,                     //!< Server time is not received from the server.
    FIEAG_NO_ACCESS_TOKEN_EXISTS,             //!< No access token exists in #mcl_core_t handle.
    FIEAG_NO_ACCESS_TOKEN_PROVIDED,           //!< Neither initial access token nor loading/saving credentials callback functions are provided.
    FIEAG_NO_FILE_SUPPORT,                    //!< The system does not support file handling.
    FIEAG_OPERATION_NOT_SUPPORTED,            //!< Requested operation is not supported.

    // HTTPS return codes.
    FIEAG_COULD_NOT_RESOLVE_PROXY,            //!< Proxy host name given as a configuration parameter could not be resolved.
    FIEAG_COULD_NOT_RESOLVE_HOST,             //!< Host name given as a configuration parameter could not be resolved.
    FIEAG_COULD_NOT_CONNECT,                  //!< MCL failed to connect to the host or proxy.
    FIEAG_SSL_HANDSHAKE_FAIL,                 //!< A problem occured during SSL/TLS handshake.
    FIEAG_NETWORK_SEND_FAIL,                  //!< A problem occured when sending data to the network.
    FIEAG_NETWORK_RECEIVE_FAIL,               //!< A problem occured when receiving data from the network.
    FIEAG_SERVER_CERTIFICATE_NOT_VERIFIED,    //!< Mindsphere certificate was not verified.
    FIEAG_IMPROPER_CERTIFICATE,               //!< The server certificate provided is in improper format and it can not be parsed.
    FIEAG_REQUEST_TIMEOUT,                    //!< The server did not respond within a timeout period.
    FIEAG_SERVER_FAIL,                        //!< Internal server error.
    FIEAG_CREATED,                            //!< If the response of server is HTTP 201.
    FIEAG_PARTIAL_CONTENT,                    //!< If the response of server is HTTP 206.
    FIEAG_BAD_REQUEST,                        //!< If the response of server is HTTP 400.
    FIEAG_UNAUTHORIZED,                       //!< If the response of server is HTTP 401.
    FIEAG_FORBIDDEN,                          //!< If the response of server is HTTP 403.
    FIEAG_NOT_FOUND,                          //!< If the response of server is HTTP 404.
    FIEAG_CONFLICT,                           //!< If the response of server is HTTP 409.
    FIEAG_PRECONDITION_FAIL,                  //!< If the response of server is HTTP 412.
    FIEAG_REQUEST_PAYLOAD_TOO_LARGE,          //!< If the response of server is HTTP 413.
    FIEAG_TOO_MANY_REQUESTS,                  //!< If the response of server is HTTP 429.
    FIEAG_UNEXPECTED_RESULT_CODE,             //!< If the response of server is unexpected.

    // Status return codes.
    FIEAG_NOT_ONBOARDED,                      //!< Agent is not onboarded to the server yet and does not possess an authentication key.
    FIEAG_ALREADY_ONBOARDED,                  //!< Agent is already onboarded to the server, hence the library did not try to onboard again.
    FIEAG_EXCHANGE_STREAMING_IS_ACTIVE,       //!< The streaming is active and shouldn't be interrupted.
    FIEAG_CREDENTIALS_UP_TO_DATE,             //!< Credentials of the mcl_core are already up to date.
    FIEAG_CANNOT_ENTER_CRITICAL_SECTION,      //!< If agent cannot enter critical section.

    // Operational return codes.
    FIEAG_BAD_CONTENT_ENCODING,               //!< If given content for Base64 encoding is bad.
    FIEAG_JSON_NON_EXISTING_CHILD,            //!< Json child which we try to get doesn't exist in the parent Json object.
    FIEAG_JSON_NAME_DUPLICATION,              //!< The same name can not be added in the same level of json object.
    FIEAG_JSON_TYPE_MISMATCH,                 //!< Type of the value of the json object does not match the type requested.
    FIEAG_SHA256_CALCULATION_FAIL,            //!< If SHA256 calculation fails.
    FIEAG_CREDENTIALS_NOT_SAVED,              //!< Credentials are not saved.
    FIEAG_CREDENTIALS_NOT_LOADED,             //!< Credentials are not loaded.

    // Internal return codes.
    FIEAG_LIST_IS_EMPTY,                      //!< There is no element in the list.
    FIEAG_LIMIT_EXCEEDED,                     //!< No more space is left to add an additional object.
    FIEAG_CORE_RETURN_CODE_END                //!< End of return codes.
} E_FIEAG_CORE_RETURN_CODE;

// Deprecated error code. Kept for backward compatibility.
#define FIEAG_NON_EXISTING_JSON_CHILD FIEAG_JSON_NON_EXISTING_CHILD

extern const char *mcl_core_return_code_strings[FIEAG_CORE_RETURN_CODE_END];

// This function converts the given return code to its string value for core module.
#define FIEAG_CORE_CODE_TO_STRING(code) (code < FIEAG_CORE_RETURN_CODE_END ? mcl_core_return_code_strings[code] : NULL)

/**
 *
 * This is a callback function prototype to load credentials for Shared Secret security profile.
 *
 * @param [out] client_id Client ID.
 * @param [out] client_secret Client secret.
 * @param [out] registration_access_token Registration access token.
 * @param [out] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#FIEAG_OK in case of success, all parameters must be dynamically allocated.</li>
 * <li>#FIEAG_CREDENTIALS_NOT_LOADED if credentials can not be loaded, all parameters must be set to NULL.</li>
 * </ul>
 */
typedef int (*mcl_credentials_load_shared_secret_callback_t)(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);

/**
 *
 * This is a callback function prototype to save credentials for Shared Secret security profile.
 *
 * @param [in] client_id Client ID.
 * @param [in] client_secret Client secret.
 * @param [in] registration_access_token Registration access token.
 * @param [in] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#FIEAG_OK in case of success.</li>
 * <li>#FIEAG_CREDENTIALS_NOT_SAVED if credentials can not be saved.</li>
 * </ul>
 */
typedef int (*mcl_credentials_save_shared_secret_callback_t)(const char *client_id, const char *client_secret, const char *registration_access_token,
    const char *registration_uri);

/**
 * This is a callback function prototype to load credentials for RSA security profile.
 *
 * @param [out] client_id Client ID.
 * @param [out] public_key Public key.
 * @param [out] private_key Private key.
 * @param [out] registration_access_token Registration access token.
 * @param [out] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#FIEAG_OK in case of success, all parameters must be dynamically allocated.</li>
 * <li>#FIEAG_CREDENTIALS_NOT_LOADED if credentials can not be loaded, all parameters must be set to NULL.</li>
 * </ul>
 */
typedef int (*mcl_credentials_load_rsa_callback_t)(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri);

/**
 * This is a callback function prototype to save credentials for RSA security profile.
 *
 * @param [in] client_id Client ID.
 * @param [in] public_key Public key.
 * @param [in] private_key Private key.
 * @param [in] registration_access_token Registration access token.
 * @param [in] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#FIEAG_OK in case of success.</li>
 * <li>#FIEAG_CREDENTIALS_NOT_SAVED if credentials can not be saved.</li>
 * </ul>
 */
typedef int (*mcl_credentials_save_rsa_callback_t)(const char *client_id, const char *public_key, const char *private_key, const char *registration_access_token,
    const char *registration_uri);

/**
 * This union is used to assign a callback function for loading credentials (whether RSA or shared secret).
 */
typedef union mcl_credentials_load_callback_t
{
    mcl_credentials_load_shared_secret_callback_t shared_secret; //!< Callback type to load shared secret.
    mcl_credentials_load_rsa_callback_t rsa;                     //!< Callback type to load RSA key.
} mcl_credentials_load_callback_t;

/**
 * This union is used to assign a callback function for saving credentials (whether RSA or shared secret).
 */
typedef union mcl_credentials_save_callback_t
{
    mcl_credentials_save_shared_secret_callback_t shared_secret; //!< Callback type to save shared secret.
    mcl_credentials_save_rsa_callback_t rsa;                     //!< Callback type to save RSA key.
} mcl_credentials_save_callback_t;

/**
 *
 * This is a callback function prototype to enter critical section (onboarding, key rotation, updating security information).
 *
 * @return
 * <ul>
 * <li>#FIEAG_OK in case of success.</li>
 * <li>#FIEAG_CANNOT_ENTER_CRITICAL_SECTION if cannot enter critical section.</li>
 * </ul>
 */
typedef int (*mcl_critical_section_enter_callback_t)(void);

/**
 *
 * This is a callback function prototype to leave critical section (onboarding, key rotation, updating security information).
 *
 */
typedef void (*mcl_critical_section_leave_callback_t)(void);

#ifdef  __cplusplus
}
#endif

#endif //FIEAG_COMMON_H_
