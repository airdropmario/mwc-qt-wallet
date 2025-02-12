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

#ifndef MWC_QT_WALLET_TASKSEND_H
#define MWC_QT_WALLET_TASKSEND_H

#include "../mwc713task.h"
#include "../../util/stringutils.h"

namespace wallet {

// Listening for transaction task
class TaskSlatesListener : public Mwc713Task {
public:
    TaskSlatesListener( MWC713 * wallet713) :
            Mwc713Task("TaskSlatesListener", "", wallet713, "") {}

    virtual ~TaskSlatesListener() override {}

    virtual bool processTask(const QVector<WEvent> & events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>();}
};

////////////////////////////////////     set-recv       ///////////////

class TaskSetReceiveAccount : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 1000*2;

    TaskSetReceiveAccount( MWC713 *wallet713, QString account, QString password ) :
            Mwc713Task("TaskSetReceiveAccount",
                       QString("set-recv ") + util::toMwc713input(account) + " -p " + util::toMwc713input(password),
                       wallet713, "set-recv for " + account) {}

    virtual ~TaskSetReceiveAccount() override {}

    virtual bool processTask(const QVector<WEvent> &events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>{ WALLET_EVENTS::S_READY };}
};


//////////////////////////////////////////////////////////////////

class TaskSendMwc : public QObject, public Mwc713Task {
    Q_OBJECT
public:
    const static int64_t TIMEOUT = 1000*60; // 1 minute should be enough

    // coinNano == -1  - mean All
    TaskSendMwc( MWC713 *wallet713, int64_t coinNano, const QString & address, QString message, int inputConfirmationNumber, int changeOutputs ) :
            Mwc713Task("TaskSendMwc",
                    buildCommand( coinNano, address, message, inputConfirmationNumber, changeOutputs),
                    wallet713, ""), sendMwcNano(coinNano) {}

    virtual ~TaskSendMwc() override {}

    virtual bool processTask(const QVector<WEvent> &events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>{ WALLET_EVENTS::S_READY };}
private:
    // coinNano == -1  - mean All
    QString buildCommand(int64_t coinNano, const QString & address, QString message="", int inputConfirmationNumber=10, int changeOutputs=1) const;

    int64_t sendMwcNano;
};


class TaskSendFile : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 1000*20;

    TaskSendFile( MWC713 *wallet713, int64_t coinNano, QString message, QString fileTx, int inputConfirmationNumber, int changeOutputs ) :
            Mwc713Task("TaskSendFile",
                       buildCommand(coinNano, message, fileTx, inputConfirmationNumber, changeOutputs),
                       wallet713, "") {}

    virtual ~TaskSendFile() override {}

    virtual bool processTask(const QVector<WEvent> &events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>{ WALLET_EVENTS::S_READY };}
private:
    QString buildCommand( int64_t coinNano, QString message, QString fileTx, int inputConfirmationNumber, int changeOutputs ) const;
};

class TaskReceiveFile : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 1000*20;

    TaskReceiveFile( MWC713 *wallet713, QString fileName, QString identifier = "" ) :
            Mwc713Task("TaskReceiveFile", buildCommand(fileName, identifier), wallet713,""), inFileName(fileName) {}

    virtual ~TaskReceiveFile() override {}

    virtual bool processTask(const QVector<WEvent> &events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>{ WALLET_EVENTS::S_READY };}
private:
    QString buildCommand(QString fileName, QString identifier) const;
    QString inFileName;
};

class TaskFinalizeFile : public Mwc713Task {
public:
    const static int64_t TIMEOUT = 1000*20;

    TaskFinalizeFile( MWC713 *wallet713, QString fileTxResponse ) :
            Mwc713Task("TaskReceiveFile",  QString("finalize --file ") + util::toMwc713input(fileTxResponse), wallet713, "") {}

    virtual ~TaskFinalizeFile() override {}

    virtual bool processTask(const QVector<WEvent> &events) override;

    virtual QSet<WALLET_EVENTS> getReadyEvents() override {return QSet<WALLET_EVENTS>{ WALLET_EVENTS::S_READY };}
};

}

#endif //MWC_QT_WALLET_TASKSEND_H
