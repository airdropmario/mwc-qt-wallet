// Copyright 2019 The MWC Developers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "Notification.h"
#include "../control/messagebox.h"
#include "../core/global.h"
#include "../util/Log.h"
#include <QSet>
#include <QVector>

namespace notify {

// Message that will be requlified from Critical to Info
static QSet<QString> falseCriticalMessages{"keybase not found! consider installing keybase locally first."};

const int MESSAGE_SIZE_LIMIT = 1000;
static QVector<NotificationMessage> notificationMessages;

// Enum to string
QString toString(MESSAGE_LEVEL level) {
    switch (level) {
        case MESSAGE_LEVEL::FATAL_ERROR:
            return "FATAL ERROR";
        case MESSAGE_LEVEL::CRITICAL:
            return "CRITICAL";
        case MESSAGE_LEVEL::WARNING:
            return "WARNING";
        case MESSAGE_LEVEL::INFO:
            return "INFO";
        case MESSAGE_LEVEL::DEBUG:
            return "DEBUG";
    }
}


////////////////////////////////////////////////////////////////////////////////////
//      NotificationMessage

QString NotificationMessage::getLevelStr() const {
    switch (level) {
        case MESSAGE_LEVEL::FATAL_ERROR: return "Err";
        case MESSAGE_LEVEL::CRITICAL:  return "Crit";
        case MESSAGE_LEVEL::WARNING:   return "Warn";
        case MESSAGE_LEVEL::INFO:      return "info";
        case MESSAGE_LEVEL::DEBUG:     return "dbg";
        default:   Q_ASSERT(false); return "???";
    }
}

QString NotificationMessage::getLevelLongStr() const {
    switch (level) {
        case MESSAGE_LEVEL::FATAL_ERROR: return "Error";
        case MESSAGE_LEVEL::CRITICAL:  return "Critical";
        case MESSAGE_LEVEL::WARNING:   return "Warning";
        case MESSAGE_LEVEL::INFO:      return "Info";
        case MESSAGE_LEVEL::DEBUG:     return "Debug";
        default:   Q_ASSERT(false); return "???";
    }
}

// To debug string
QString NotificationMessage::toString() const {
    return ( "NotifMsg(level=" + getLevelStr() + ", message="+message + ")" );
}

////////////////////////////////////////////////////////////////////////////////////
//      Notification

Notification::Notification() {}

static Notification * singletineNotification = nullptr;

Notification * Notification::getObject2Notify() {
    if (singletineNotification == nullptr) {
        singletineNotification = new Notification();
    }
    return singletineNotification;
}

void Notification::sendNewNotificationMessage(MESSAGE_LEVEL level, QString message) {
    emit onNewNotificationMessage(level, message);
}


// Get all notification messages
// Check signal: Notification::onNewNotificationMessage
QVector<NotificationMessage> getNotificationMessages() {
    return notificationMessages;
}

// Generic. Reporting fatal error that somebody will process and exit app
void reportFatalError( QString message )  {
    appendNotificationMessage( MESSAGE_LEVEL::FATAL_ERROR, message );
}

void appendNotificationMessage( MESSAGE_LEVEL level, QString message ) {

    logger::logInfo("Notification", toString(level) + "  " + message );

    if (level == MESSAGE_LEVEL::FATAL_ERROR) {
        // Fatal error. Display message box and exiting. We don't want to continue
        control::MessageBox::messageText(nullptr, "Wallet Error", "Wallet got a critical error:\n" + message + "\n\nPress OK to exit the wallet" );
        mwc::closeApplication();
        return;
    }

    // Message is not fatal, adding it into the logs
    if (level == MESSAGE_LEVEL::CRITICAL) {
        if ( falseCriticalMessages.contains(message) )
            level = MESSAGE_LEVEL::INFO;
    }

    NotificationMessage msg(level, message);

    // check if it is duplicate message. Duplicates will be ignored.
    if (! ( notificationMessages.size()>0 && notificationMessages.last().message == message ) ) {
        notificationMessages.push_back(msg);

        while (notificationMessages.size() > MESSAGE_SIZE_LIMIT)
            notificationMessages.pop_front();
    }

    logger::logEmit( "MWC713", "onNewNotificationMessage", msg.toString() );

    Notification::getObject2Notify()->sendNewNotificationMessage(msg.level, msg.message);
}

}
