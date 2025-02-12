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

#include "statemachine.h"
#include "a_initaccount.h"
#include "a_inputpassword.h"
#include "../core/appcontext.h"
#include "../core/mainwindow.h"
#include "k_accounts.h"
#include "k_AccountTransfer.h"
#include "x_events.h"
#include "hodl.h"
#include "e_Receive.h"
#include "e_listening.h"
#include "e_transactions.h"
#include "e_outputs.h"
#include "w_contacts.h"
#include "x_walletconfig.h"
#include "m_airdrop.h"
#include "a_StartWallet.h"
#include "g_Send.h"
#include "x_ShowSeed.h"
#include "x_Resync.h"
#include "../core/Config.h"
#include "u_nodeinfo.h"
#include "g_Finalize.h"


namespace state {

StateMachine::StateMachine(StateContext * _context) :
        context(_context)
{
    context->setStateMachine(this);

    states[ STATE::START_WALLET ]   = new StartWallet(context);
    states[ STATE::STATE_INIT ]     = new InitAccount(context);
    states[ STATE::INPUT_PASSWORD ] = new InputPassword(context);
    states[ STATE::ACCOUNTS ]       = new Accounts(context);
    states[ STATE::ACCOUNT_TRANSFER ] = new AccountTransfer(context);
    states[ STATE::EVENTS ]         = new Events(context);
    states[ STATE::HODL ]           = new Hodl(context);
    states[ STATE::SEND ]           = new Send(context);
    states[ STATE::RECEIVE_COINS ]  = new Receive(context);
    states[ STATE::LISTENING ]      = new Listening(context);
    states[ STATE::TRANSACTIONS ]   = new Transactions(context);
    states[ STATE::OUTPUTS ]        = new Outputs(context);
    states[ STATE::CONTACTS ]       = new Contacts(context);
    states[ STATE::WALLET_CONFIG ]  = new WalletConfig(context);
    states[ STATE::AIRDRDOP_MAIN ]  = new Airdrop(context);
    states[ STATE::SHOW_SEED ]      = new ShowSeed(context);
    states[ STATE::RESYNC ]         = new Resync(context);
    states[ STATE::NODE_INFO ]      = new NodeInfo(context);
    states[ STATE::FINALIZE ]       = new Finalize(context);

    startTimer(1000);
}

StateMachine::~StateMachine() {
    for (auto st : states)
        delete st;

    states.clear();
}

// Please use carefully, don't abuse this interface since no type control can be done
State * StateMachine::getState(STATE state) const {
    return states.value(state, nullptr);
}

State* StateMachine::getCurrentStateObj() const {
    return getState(currentState);
}


void StateMachine::start() {

    // Init the app
    for ( auto it = states.begin(); it!=states.end(); it++)
    {
        if (processState(it.value()))
            continue;

        currentState = it.key();
        context->mainWnd->updateActionStates(currentState);
        break;
    }
}

void StateMachine::executeFrom( STATE nextState ) {

    // notify current state
    State* prevState = states.value(currentState, nullptr);
    if (prevState)
        prevState->exitingState();

    if (nextState == STATE::NONE)
        nextState = states.firstKey();

    if ( isLogoutOff(nextState ) )
        logoutTime = 0;

    Q_ASSERT( states.contains(nextState) );

    currentState = STATE::NONE;
    for ( auto it = states.find(nextState); it!=states.end(); it++)
    {
        if ( processState( it.value() ) )
            continue;

        currentState = it.key();
        context->mainWnd->updateActionStates(currentState);
        break;
    }

    // Resync is blocking logout. We need to respect that.
    if ( !isLogoutOff(currentState) )
        resetLogoutLimit();

    if (currentState == STATE::NONE) {
        // Selecting the send page if nothing found
        context->appContext->setActiveWndState( STATE::SEND );
        executeFrom(STATE::NONE);
    }

}

bool StateMachine::setActionWindow( STATE actionWindowState, bool enforce ) {
    if (!enforce && !isActionWindowMode() )
            return false;

    context->appContext->setActiveWndState(actionWindowState);
    executeFrom(actionWindowState);
    return currentState==actionWindowState;
}

STATE StateMachine::getActionWindow() const {
    return context->appContext->getActiveWndState();
}

// return true if action window will applicable
bool StateMachine::isActionWindowMode() const {
    return context->appContext->getActiveWndState() == currentState;
}

// Reset logout time.
void StateMachine::resetLogoutLimit() {
    logoutTime = QDateTime::currentMSecsSinceEpoch() + config::getLogoutTimeMs();
    blockLogoutCounter = 0;
}

// Logout must be blocked for modal dialogs
void StateMachine::blockLogout() {
    blockLogoutCounter++;
    logoutTime = 0;
}
void StateMachine::unblockLogout() {
    blockLogoutCounter--;
    if (blockLogoutCounter<=0 && logoutTime==0)
        resetLogoutLimit();
}

//////////////////////////////////////////////////////////////////////////

bool StateMachine::processState(State* st) {
    NextStateRespond resp = st->execute();
    if (resp.result == NextStateRespond::RESULT::DONE)
        return true;

    if (resp.result == NextStateRespond::RESULT::WAIT_FOR_ACTION)
        return false;

    Q_ASSERT(false); // NONE state is a bug

    return false;
}

void StateMachine::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);
    // Check if timer expired and we need to logout...
    if (logoutTime==0)
        return;

    if (QDateTime::currentMSecsSinceEpoch() > logoutTime ) {
        // logout
        logoutTime = 0;
        context->appContext->pushCookie<QString>("LockWallet", "lock");
        context->stateMachine->executeFrom(STATE::NONE);
    }
}


}
