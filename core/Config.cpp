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

#include "Config.h"

namespace config {

static QString mwc713conf;
static QString mwcGuiWalletConf;
static QString mwcPath;
static QString wallet713path;
static QString mainStyleSheetPath;
static QString dialogsStyleSheetPath;
static QString airdropUrl;
static QString airdropUrlMainNetUrl;
static QString airdropUrlTestNetUrl;
static int64_t logoutTimeMs = 1000*60*15; // 15 minutes is default
static double  timeoutMultiplier = 1.0;
static bool    useMwcMqS = true;
static int     sendTimeoutMs = 60000; // 1 minute

void setMwc713conf( QString conf ) {
    mwc713conf = conf;
}
void setMwcGuiWalletConf( QString conf ) {
    mwcGuiWalletConf = conf;
}

/**
 * COnfiguration for mwc-mq-wallet
 * @param mwcPath               - path to mwc-node.  Not used now
 * @param wallet713path         - path to mwc713
 * @param mainStyleSheetPath    - path to main (non modal) stylesheet
 * @param dialogsStyleSheetPath - path to modal dialogs stylesheet (changes set to the main stylesheet)
 * @param airdropUrlMainNetUrl  - Airdrop server claims URL for main net
 * @param airdropUrlTestNetUrl  - Airdrop server claims URL for floo net
 * @param logoutTimeMs          - Automatic locking for the wallet
 * @param timeoutMultiplier     - Multiplier for jobs timeouts. Default is 1.0
 * @param useMwcMqS             - true: use mwc mqs for slates exchange.  false: using mwc mq (non secure grin box) for slates exchange
 * @param sendTimeoutMs         - timeout for mwc mq send. Expected that 2nd party is online. Otherwise we will ask user if he want to stop waiting and cancel transaction.
 */
void setConfigData(QString _mwcPath, QString _wallet713path,
                   QString _mainStyleSheetPath, QString _dialogsStyleSheetPath,
                   QString _airdropUrlMainNetUrl, QString _airdropUrlTestNetUrl,
                   int64_t  _logoutTimeMs,
                   double _timeoutMultiplier,
                   bool _useMwcMqS,
                   int _sendTimeoutMs) {
    mwcPath = _mwcPath;
    wallet713path = _wallet713path;
    mainStyleSheetPath = _mainStyleSheetPath;
    dialogsStyleSheetPath = _dialogsStyleSheetPath;
    airdropUrlMainNetUrl = _airdropUrlMainNetUrl;
    airdropUrlTestNetUrl = _airdropUrlTestNetUrl;
    logoutTimeMs = _logoutTimeMs;
    timeoutMultiplier = _timeoutMultiplier;
    useMwcMqS = _useMwcMqS;
    sendTimeoutMs = _sendTimeoutMs;
}


// Note, workflow for config not enforced. Please don't abuse it
const QString & getMwc713conf() {return mwc713conf;}
const QString & getMwcGuiWalletConf() {return mwcGuiWalletConf;}

const QString & getMwcpath() {return mwcPath;}
const QString & getWallet713path() {return wallet713path;}
const QString & getMainStyleSheetPath() {return mainStyleSheetPath;}
const QString & getDialogsStyleSheetPath() {return dialogsStyleSheetPath;}
const QString & getAirdropMainNetUrl() {return airdropUrlMainNetUrl;}
const QString & getAirdropTestNetUrl() {return airdropUrlTestNetUrl;}
int64_t         getLogoutTimeMs() {return logoutTimeMs;}


double          getTimeoutMultiplier() {return timeoutMultiplier;}
void            increaseTimeoutMultiplier() { timeoutMultiplier *= 1.6; }

bool            getUseMwcMqS() {return useMwcMqS;}

int             getSendTimeoutMs() {return sendTimeoutMs;}


QString toString() {
    return "mwc713conf=" + mwc713conf + "\n" +
            "mwcGuiWalletConf=" + mwcGuiWalletConf + "\n" +
            "mwcPath=" + mwcPath + "\n" +
            "wallet713path=" + wallet713path + "\n" +
            "mainStyleSheetPath=" + mainStyleSheetPath + "\n" +
            "dialogsStyleSheetPath=" + dialogsStyleSheetPath + "\n" +
            "useMwcMqS=" + (useMwcMqS?"true":"false") + "\n" +
            "sendTimeoutMs=" + QString::number(sendTimeoutMs) + "\n";
}


}
