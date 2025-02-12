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

#include "Process.h"
#include <QProcess>
#include "../core/Config.h"
#include "../control/messagebox.h"
#include <QSysInfo>

namespace util {


bool processWaitForFinished( QProcess * process, int timeoutMs, const QString & processName ) {

    if (!process->waitForFinished((int) (timeoutMs * config::getTimeoutMultiplier())) ) {

        if (process->state() == QProcess::QProcess::NotRunning)
            return true;

        QProcess::ProcessError errCode = process->error();
        if (errCode == QProcess::Timedout) {
            if (control::MessageBox::questionText(nullptr, "Warning", "Stopping process " + processName +
                                                                   " is taking longer than expected.\nContinue to wait?",
                                              "Yes", "No", true, false) == control::MessageBox::RETURN_CODE::BTN1) {
                config::increaseTimeoutMultiplier();
                return processWaitForFinished(process, timeoutMs, processName);
            }
        }

        return false;
    }

    return true;
}

bool isBuild64Bit() {
    static bool b = QSysInfo::buildCpuArchitecture().contains(QLatin1String("64"));
    return b;
}

}


