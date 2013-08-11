/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * An appender to write log4cxx log to scribe
 * contact: qingqibai@gmail.com
 */

#ifndef _LOG4CXX_SCRIBE_APPENDER_H
#define _LOG4CXX_SCRIBE_APPENDER_H

#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable: 4231 4251 4275 4786 )
#endif


#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/appenderattachableimpl.h>
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/helpers/thread.h>
#include <log4cxx/helpers/mutex.h>
#include <log4cxx/helpers/condition.h>
#include <scribe_types.h>
#include <scribe.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace boost;
using namespace scribe::thrift;

//using namespace scribe;
namespace log4cxx
{

        /**
        The ScribeAppender lets users log events to scribe. 
        */
        class LOG4CXX_EXPORT ScribeAppender : public virtual AppenderSkeleton
        {
        public:
                /**
                The default port number of remote logging scribe server (1463).
                */
                //static int DEFAULT_PORT;

                //static int DEFAULT_HOST;

                DECLARE_LOG4CXX_OBJECT(ScribeAppender)
                BEGIN_LOG4CXX_CAST_MAP()
                        LOG4CXX_CAST_ENTRY(ScribeAppender)
                        LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
                END_LOG4CXX_CAST_MAP()

                /**
                 * Create new instance.
                */
                ScribeAppender();
                
                /**
                 *  Destructor.
                 */
                virtual ~ScribeAppender();

                int getDefaultPort() const;

                LogString getDefaultHost() const;
                
                LogString getDefaultCategory() const;

                LogString getScribeHost() const;

                void setScribeHost(const LogString &host);

                int getScribePort() const;

                void setScribePort(const int port);

                LogString getScribeCategory() const;

                void setScribeCategory(const LogString &category);

                LogString getHostname() const;

                void setHostname(const LogString &hostname);

                ScribeAppender(const LogString& host, int port);

                /**
                Connects to remote scribe server at <code>host</code> and <code>port</code>.
                */
                void configureScribe();

                /**
                 Appends a log message to Scribe
                */
                void append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p);

                /**
                 Connect to scribe if not open, reconnect if failed.
                */
                void connect();

                void close();
                
                void setOption(const LogString& option, const LogString& value);

                virtual bool requiresLayout() const;

        protected:
                LogString scribeHost;
                int scribePort;
                LogString scribeCategory;
                LogString scribeHostname;
                shared_ptr<TTransport> transport;
                shared_ptr<scribeClient> client;

        }; // class ScribeAppender
        LOG4CXX_PTR_DEF(ScribeAppender);
}  //  namespace log4cxx

#if defined(_MSC_VER)
#pragma warning ( pop )
#endif


#endif//  _LOG4CXX_ASYNC_APPENDER_H
