#ifndef SOMEIP_MESSAGE_H
#define SOMEIP_MESSAGE_H

#include <stdint.h>
#include <stdexcept>
#include <vector>
#include <limits>
#include "../../helper/payload_helper.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            /// @brief SOME/IP communication message type
            enum class SomeIpMessageType : uint8_t
            {
                // represent request for method to run at service provider
                Request = 0x00,           ///< Request expecting a response
                RequestNoReturn = 0x01,   ///< Fire and forget request
                
                // represent response to request for method
                Response = 0x80,          ///< Response without any error
                Error = 0x81,             ///< Response containing error

                // for any SOMEIP/SD message or server sends data to already subscriper client
                Notification = 0x02,      ///< Notification/event callback
                
                TpRequest = 0x20,         ///< Transfer protocol request
                TpRequestNoReturn = 0x21, ///< Transfer protocol fire and forget request
                TpNotification = 0x22,    ///< Transfer protocol notification
                TpResponse = 0xa0,        ///< Transfer protocol response
                TpError = 0xa1            ///< Transfer protocol error
            };

            /// @brief SOME/IP communication message return code
            enum class SomeIpReturnCode : uint8_t
            {
                // set for all messages except messages whose MessageType is ERROR
                eOK,                    ///< No error occurred
                
                // set for all messages whose MessageType is ERROR
                eNotOk,                 ///< Unspecified error occurred
                eUnknownService,        ///< Service ID is unknown
                eUnknownMethod,         ///< Method ID is unknown
                eNotReady,              ///< Service is not running
                eNotReachable,          ///< Service is not reachable
                eTimeout,               ///< Timeout occurred
                eWrongProtocolVersion,  ///< SOME/IP protocol version is not supported
                eWrongInterfaceVersion, ///< Service interface version is not supported
                eMalformedMessage,      ///< Deserialization error occurred
                eWrongMessageType,      ///< Invalid message type
                
                eE2eRepeated,           ///< Repeated E2E calculation error
                eE2eWrongSequnece,      ///< Wrong E2E sequence error
                eE2e,                   ///< Unspecified E2E error
                eE2eNotAvailable,       ///< E2E is not supported
                eE2eNoNewData           ///< No new data E2E calculation present
            };

            /// @brief Abstract class represents SOME/IP Message  
            class SomeIpMessage
            {
            private:
                /******************************* attributes ******************************/

                uint32_t mMessageId;
                uint16_t mClientId;
                uint16_t mSessionId;
                uint8_t mProtocolVersion;
                uint8_t mInterfaceVersion;
                SomeIpMessageType mMessageType;
                SomeIpReturnCode mReturnCode;


               /****** it helps in constructors that will be inherited to mychild *******/

                SomeIpMessage(uint32_t messageId,
                              uint16_t clientId,
                              uint16_t sessionId,
                              uint8_t protocolVersion,
                              uint8_t interfaceVersion,
                              SomeIpMessageType messageType,
                              SomeIpReturnCode returnCode) noexcept;

            protected:
                /***************************** for my child ******************************/

                /// @brief Constructor for request and notification
                /// @param messageId Message ID consisting service and method/event ID
                /// @param clientId Client ID including ID prefix
                /// @param protocolVersion SOME/IP protocol header version
                /// @param interfaceVersion Service interface version
                /// @param messageType SOME/IP message type (request or notification)
                /// @param sessionId Active/non-active session ID
                SomeIpMessage(uint32_t messageId,
                              uint16_t clientId,
                              uint8_t protocolVersion,
                              uint8_t interfaceVersion,
                              SomeIpMessageType messageType,
                              uint16_t sessionId = 1);

                /// @brief Constructor for response and error
                /// @param messageId Message ID consisting service and method/event ID
                /// @param clientId Client ID including ID prefix
                /// @param protocolVersion SOME/IP protocol header version
                /// @param interfaceVersion Service interface version
                /// @param messageType SOME/IP message type (response or error)
                /// @param returnCode Message response/error return code
                /// @param sessionId Active/non-active session ID
                SomeIpMessage(uint32_t messageId,
                              uint16_t clientId,
                              uint8_t protocolVersion,
                              uint8_t interfaceVersion,
                              SomeIpMessageType messageType,
                              SomeIpReturnCode returnCode,
                              uint16_t sessionId = 1);

                /// @brief Deserialize a SOME/IP message payload
                /// @param message SOME/IP message to be filled by deserializing the payload
                /// @param payload Serialized SOME/IP message payload byte array
                /*
                    usually vector functions may throw exception
                */
                static void Deserialize(
                    SomeIpMessage *message,
                    const std::vector<uint8_t> &payload);

            public:
                /**************** move constructor and move assigment operator **************/
                
                SomeIpMessage(SomeIpMessage&& other) noexcept;

                SomeIpMessage& operator=(SomeIpMessage&& other);



                /********************************** getters *********************************/
                
                /// @brief Get message ID
                /// @returns Message ID consisting service and method/event ID
                uint32_t MessageId() const noexcept;

                /// @brief Get client ID
                /// @returns Client ID including ID prefix
                uint16_t ClientId() const noexcept;

                /// @brief Get session ID
                /// @returns Active/non-active session ID
                uint16_t SessionId() const noexcept;

                /// @brief Get protocol version
                /// @returns SOME/IP protocol header version
                uint8_t ProtocolVersion() const noexcept;

                /// @brief Get interface version
                /// @returns Service interface version
                uint8_t InterfaceVersion() const noexcept;

                /// @brief Get message type
                /// @returns SOME/IP message type
                SomeIpMessageType MessageType() const noexcept;

                /// @brief Get return code
                /// @returns SOME/IP message return code
                SomeIpReturnCode ReturnCode() const noexcept;



                /********************************** pure virtual functions **************************/
                
                /// @brief Get message length
                /// @returns Message length including the payload length
                virtual uint32_t Length() const noexcept = 0;



                /********************************** non-pure virtual functions ***********************/

                /// @brief Increment the session ID by one
                /// @returns True if the session ID is wrappered; otherwise false
                /// @note In the case of wrapping, session ID will start from 1
                virtual bool IncrementSessionId() noexcept;

                /// @brief Set a new session ID
                /// @param sessionId New session ID
                /*
                    we have intend to make this function throw execption
                    if user pass sessionId = 0 in case someip/sd message only
                */
                virtual void SetSessionId(uint16_t sessionId);

                /// @brief Get message payload
                /// @returns Byte array
                /*
                    usually vector functions may throw exception
                */
                virtual std::vector<uint8_t> Payload() const;

                /// @brief print message
                virtual void print() const noexcept;

                

                /****************** tell compiler to generate default desctructor  ********************/

                virtual ~SomeIpMessage() noexcept = default;
            };
        }
    }
}

#endif