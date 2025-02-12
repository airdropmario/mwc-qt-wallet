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

#ifndef MWC_QT_WALLET_TASKWALLET_H
#define MWC_QT_WALLET_TASKWALLET_H

#include "../mwc713task.h"

namespace wallet {

QVector<QString> calcSeedFromEvents(const QVector<WEvent> &events);

class TaskInit : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 5000;

    TaskInit(MWC713 *wallet713) :
            Mwc713Task("TaskInit", "", wallet713, "") {} // no input for init

    virtual ~TaskInit() override {}

    virtual bool processTask(const QVector<WEvent> &events) override;

    virtual void onStarted() override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {
        return QSet<WALLET_EVENTS>{WALLET_EVENTS::S_INIT_WANT_ENTER};
    }

};

class TaskStop : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 3000;

    TaskStop(MWC713 *wallet713) :
            Mwc713Task("TaskStop", " ", wallet713, "") {} // no input for init

    virtual ~TaskStop() override {}

    virtual bool processTask(const QVector<WEvent> &events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {
        return QSet<WALLET_EVENTS>{WALLET_EVENTS::S_READY};
    }

};

class TaskUnlock : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 5000;

    TaskUnlock( MWC713 * wallet713, QString password) :
            Mwc713Task("Unlocking", buildWalletRequest(password), wallet713, "unlock") {}

    virtual ~TaskUnlock() override {}

    virtual bool processTask(const QVector<WEvent> & events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>{ WALLET_EVENTS::S_PASSWORD_ERROR, WALLET_EVENTS::S_READY };}

private:
    static QString buildWalletRequest(QString password);
};

// InitW is similar to unlock
class TaskInitW : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 5000;

    TaskInitW( MWC713 * wallet713, QString password) :
            Mwc713Task("init -p", "init -p " + password, wallet713, "init with password (unlock)") {}

    virtual ~TaskInitW() override {}

    virtual bool processTask(const QVector<WEvent> & events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>{ WALLET_EVENTS::S_INIT_WANT_ENTER, WALLET_EVENTS::S_READY };}

};

// InitW is similar to unlock
class TaskInitWpressEnter : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 1000;

    TaskInitWpressEnter( MWC713 * wallet713) :
            Mwc713Task("pressEnter", " ", wallet713, "") {}

    virtual ~TaskInitWpressEnter() override {}

    virtual bool processTask(const QVector<WEvent> & events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>{ WALLET_EVENTS::S_READY };}

};

// Logout from the wallet. Close that app
class TaskLogout: public Mwc713Task {
public:
const static int64_t TIMEOUT = -1; // start now

TaskLogout( MWC713 * wallet713) :
        Mwc713Task("Logout", "", wallet713, "") {} // press enter to trigger ready.

virtual ~TaskLogout() override {}

virtual bool processTask(const QVector<WEvent> & events) override;

virtual QSet<WALLET_EVENTS> getReadyEvents() override {return {};}
};

// get next key is end the wallet life. It is similar to logout
class TaskGetNextKey: public Mwc713Task {
public:
const static int64_t TIMEOUT = 3000; // execute now

TaskGetNextKey( MWC713 * wallet713, int64_t amountNano, QString _btcaddress, QString _airDropAccPassword) :
        Mwc713Task("TaskGetNextKey","getnextkey --amount " + QString::number(amountNano), wallet713, "getnextkey is executed"), btcaddress(_btcaddress), airDropAccPassword(_airDropAccPassword) {}

virtual ~TaskGetNextKey() override {}

virtual bool processTask(const QVector<WEvent> & events) override;

virtual QSet<WALLET_EVENTS> getReadyEvents() override {return { WALLET_EVENTS::S_READY };}
private:
    QString btcaddress;
    QString airDropAccPassword;
};

// node info command
class TaskNodeInfo: public Mwc713Task {
public:
    const static int64_t TIMEOUT = 30000; // execute now

    TaskNodeInfo( MWC713 * wallet713) :
            Mwc713Task("TaskNodeInfo","nodeinfo", wallet713, "") {}

    virtual ~TaskNodeInfo() override {}

    virtual bool processTask(const QVector<WEvent> & events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return { WALLET_EVENTS::S_READY };}
};


}

#endif //MWC_QT_WALLET_TASKWALLET_H
