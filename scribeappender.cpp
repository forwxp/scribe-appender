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

#if defined(_MSC_VER)
#pragma warning ( disable: 4231 4251 4275 4786 )
#endif

#include <log4cxx/scribeappender.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/optionconverter.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/helpers/synchronized.h>
#include <apr_time.h>
#include <apr_atomic.h>
#include <apr_thread_proc.h>
#include <string.h>


IMPLEMENT_LOG4CXX_OBJECT(ScribeAppender)


// The default port number of remote scribe logging server (4560)
//int ScribeAppender::DEFAULT_PORT                 = 1463;

// The default reconnection delay (30000 milliseconds or 30 seconds).
//LogString ScribeAppender::DEFAULT_HOST   = "127.0.0.1";



ScribeAppender::ScribeAppender() {
    scribeHost = getDefaultHost();
    scribePort = getDefaultPort();
    scribeCategory = getDefaultCategory();
}

ScribeAppender::~ScribeAppender() { }

ScribeAppender::ScribeAppender(const LogString& host, int port) {
    scribeHost = host;
    scribePort = port;
}

LogString ScribeAppender::getDefaultHost() const {
    //return DEFAULT_PORT;
    return "127.0.0.1";
}

int ScribeAppender::getDefaultPort() const {
    //return DEFAULT_HOST;
    return 1463;
}
   
LogString ScribeAppender::getDefaultCategory() const {
    //return DEFAULT_CATEGORY;
    return "test";
}

LogString ScribeAppender::getScribeHost() const {
    return scribeHost;
}

void ScribeAppender::setScribeHost(const LogString &host) {
    this->scribeHost = host;
}

int ScribeAppender::getScribePort() const {
    return scribePort;
}

void ScribeAppender::setScribePort(const int port) {
    this->scribePort = port;
}

LogString ScribeAppender::getScribeCategory() const {
    return scribeCategory;
}

void ScribeAppender::setScribeCategory(const LogString &category) {
    this->scribeCategory = category;
}

LogString ScribeAppender::getHostname() const {
    return scribeHostname;
}

void ScribeAppender::setHostname(const LogString &hostname) {
    this->scribeHostname = hostname;
}

void ScribeAppender::configureScribe() {
    synchronized sync(mutex);
    std::string host = scribeHost;
    int port = scribePort;
    shared_ptr<TSocket> socket(new TSocket(host, port));
    transport = shared_ptr<TFramedTransport>(new TFramedTransport(socket));
    shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
    client = shared_ptr<scribeClient>(new scribeClient(protocol));
    try {
        transport->open();
    } catch(TTransportException e) {
        LogLog::warn(LOG4CXX_STR("Transport Execption while write log to scribe server: "), e);
    } catch (IOException e) {
        LogLog::warn(LOG4CXX_STR("IO Execption while write log to scribe server: "), e);
    } catch (Exception e) {
        LogLog::warn(LOG4CXX_STR("STD Execption while write log to scribe server: "), e);
    }
}

void ScribeAppender::connect() {
    if (transport != NULL && transport->isOpen()) {
        return;
    }

    if (transport != NULL && transport->isOpen() == false) {
        transport->close();
    }
    configureScribe();
}

void ScribeAppender::close() {
    if (transport != NULL && transport->isOpen()) {
        transport->close();
    }
}

void ScribeAppender::append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p) {
    connect();
    LogString msg;
    layout->format(msg, event, p);
    std::vector<LogEntry> logEntries;
    std::string mmsg = msg;
    std::string category = scribeCategory;
    try {
        synchronized sync(mutex);
        LogEntry entry;
        entry.__set_category(category);
        entry.__set_message(mmsg);
        logEntries.push_back(entry);
        client->Log(logEntries);
    } catch(TTransportException &e) {
        LogLog::warn(LOG4CXX_STR("Detected problem with connection to scribe server: "), e);
        transport->close();
    } catch(std::exception &e) {
        LogLog::warn(LOG4CXX_STR("Std Execption while write log to scribe server: "), e);
    }
}

void ScribeAppender::setOption(const LogString& option, const LogString& value) {
    if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("SCRIBEHOST"), LOG4CXX_STR("scribehost"))) {
        setScribeHost(value);
    } else if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("PORT"), LOG4CXX_STR("port"))) {
        setScribePort(OptionConverter::toInt(value, getDefaultPort()));
    } else if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("SCRIBECATEGORY"), LOG4CXX_STR("scribecategory"))) {
        setScribeCategory(value);
    } else {
        AppenderSkeleton::setOption(option, value);
    }
}

bool ScribeAppender::requiresLayout() const {
   return true;
}
