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

#include <core/global.h>
#include "u_nodeinfo_w.h"
#include "ui_u_nodeinfo_w.h"
#include "../state/u_nodeinfo.h"
#include "../control/messagebox.h"
#include "../dialogs/u_changenode.h"
#include <QScrollBar>
#include "../state/timeoutlock.h"
#include "../dialogs/u_mwcnodelogs.h"

namespace wnd {

//static
QString NodeInfo::lastShownErrorMessage;

static QString toBoldAndYellow(QString text) {
    if (text == "Ready")
        return text;

    return "<span style=\" font-weight:900; color:#CCFF33;\">" + text + "</span>";
}


NodeInfo::NodeInfo(QWidget *parent, state::NodeInfo * _state) :
        core::NavWnd(parent, _state->getContext()),
        ui(new Ui::NodeInfo),
        state(_state)
{
    ui->setupUi(this);

    ui->warningLine->hide();

    // Need simulate post message. Using events for that
    connect(this, &NodeInfo::showNodeConnectionError, this,  &NodeInfo::onShowNodeConnectionError, Qt::QueuedConnection );

    connectionType = state->getNodeConnection().first.connectionType;

    if (connectionType == wallet::MwcNodeConnection::NODE_CONNECTION_TYPE::LOCAL)
        ui->statusInfo->setText( toBoldAndYellow( state->getMwcNodeStatus() ) );

    ui->showLogsButton->setEnabled( connectionType == wallet::MwcNodeConnection::NODE_CONNECTION_TYPE::LOCAL );

    showWarning("");
}

NodeInfo::~NodeInfo() {
    state->wndIsGone(this);
    delete ui;
}


// logs to show, multi like output
void NodeInfo::updateEmbeddedMwcNodeStatus( const QString & status ) {
    if (connectionType == wallet::MwcNodeConnection::NODE_CONNECTION_TYPE::LOCAL)
        ui->statusInfo->setText( toBoldAndYellow(status) );
}

// Empty string to hide warning...
void NodeInfo::showWarning(QString warning) {
    if (currentWarning == warning)
        return;

    currentWarning = warning;

    if (warning.isEmpty()) {
        ui->lineSeparator->show();
        ui->warningLine->hide();
    }
    else {
        ui->lineSeparator->hide();
        ui->warningLine->show();
        ui->warningLine->setText(warning);
    }
}


void NodeInfo::setNodeStatus( const QString & localNodeStatus, const state::NodeStatus & status ) {
    QString warning;

    if (!status.online) {
        QString statusStr = "Offline";

        if (connectionType == wallet::MwcNodeConnection::NODE_CONNECTION_TYPE::LOCAL && localNodeStatus!="Ready") {
            statusStr = localNodeStatus;
        }

        ui->statusInfo->setText( toBoldAndYellow(statusStr) );
        ui->connectionsInfo->setText("-");
        ui->heightInfo->setText("-");
        ui->difficultyInfo->setText("-");

        if ( statusStr == "Offline" ) {
             if (lastShownErrorMessage != status.errMsg) {
                 emit showNodeConnectionError(status.errMsg);
                 lastShownErrorMessage = status.errMsg;
             }
        }
    }
    else {
        if ( status.nodeHeight + mwc::NODE_HEIGHT_DIFF_LIMIT < status.peerHeight ) {
            if (connectionType != wallet::MwcNodeConnection::NODE_CONNECTION_TYPE::LOCAL)
                ui->statusInfo->setText(toBoldAndYellow("Syncing") );
        }
        else {
            if (connectionType != wallet::MwcNodeConnection::NODE_CONNECTION_TYPE::LOCAL)
                ui->statusInfo->setText("Online");
        }

        if (status.connections <= 0) {
            ui->connectionsInfo->setText( toBoldAndYellow("None") ); // Two offline is confusing and doesn't look good. Let's keep zero and highlight it.

            if (connectionType != wallet::MwcNodeConnection::NODE_CONNECTION_TYPE::CLOUD) {
                warning = toBoldAndYellow("Please note. You can't run two mwc-node with same public IP.<br>That might be a reason why node unable to find any peers.");
            }
        }
        else {
            ui->connectionsInfo->setText( util::longLong2Str(status.connections) );
        }

        ui->heightInfo->setText( util::longLong2Str(status.nodeHeight) );
        ui->difficultyInfo->setText( util::longLong2ShortStr(status.totalDifficulty, 9) );
    }

    showWarning(warning);
}

void NodeInfo::onShowNodeConnectionError(QString errorMessage) {
    control::MessageBox::messageText(this, "mwc node connection error",
        "Unable to retrieve mwc node status.\n" + errorMessage);
}

void NodeInfo::on_refreshButton_clicked() {
    if (control::MessageBox::questionText(this, "Re-sync account with a node", "Account re-sync will validate transactions and outputs for your accounts. Re-sync can take several minutes.\nWould you like to continue",
                       "Yes", "No", false,true) == control::MessageBox::RETURN_CODE::BTN1 ) {
        state->requestWalletResync();
    }
}

void NodeInfo::on_showLogsButton_clicked()
{
    state::TimeoutLockObject to( state );

    dlg::MwcNodeLogs logsDlg(this, state->getMwcNode() );
    logsDlg.exec();
}

void NodeInfo::on_changeNodeButton_clicked()
{
    state::TimeoutLockObject to( state );

    // call dialog the allow to change the
    QPair< wallet::MwcNodeConnection, wallet::WalletConfig > conInfo = state->getNodeConnection();

    dlg::ChangeNode changeNodeDlg(this, conInfo.first, conInfo.second.getNetwork() );

    if ( changeNodeDlg.exec() == QDialog::Accepted ) {
        state->updateNodeConnection( changeNodeDlg.getNodeConnectionConfig(), conInfo.second );
    }
}

}
