/*
Copyright (C) 2013 Yubico AB.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QSettings>
#include "yubiaccbox.h"
#include "ui_yubiaccbox.h"
#include "ui/helpbox.h"
#include "ui/confirmbox.h"

YubiAccBox::YubiAccBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::YubiAccBox)
{
    ui->setupUi(this);

    QSignalMapper *mapper = new QSignalMapper(this);
    connect(ui->configProtectionHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->configProtectionHelpBtn, HelpBox::Help_ConfigurationProtection);
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(helpBtn_pressed(int)));
}

void YubiAccBox::helpBtn_pressed(int helpIndex) {
    HelpBox help(this);
    help.setHelpIndex((HelpBox::Help)helpIndex);
    help.exec();
}

YubiAccBox::~YubiAccBox()
{
    delete ui;
}

void YubiAccBox::reset() {
    ui->configProtectionCombo->setCurrentIndex(0);
}

void YubiAccBox::on_configProtectionCombo_currentIndexChanged(int index) {
   switch(index) {
      case CONFIG_PROTECTION_DISABLED:
          ui->currentAccessCodeTxt->clear();
          ui->currentAccessCodeTxt->setEnabled(false);
          ui->currentUseSerial->setChecked(false);
          ui->currentUseSerial->setEnabled(false);
          ui->newAccessCodeTxt->clear();
          ui->newAccessCodeTxt->setEnabled(false);
          ui->newUseSerial->setChecked(false);
          ui->newUseSerial->setEnabled(false);
          break;
      case CONFIG_PROTECTION_ENABLE:
          ui->currentAccessCodeTxt->clear();
          ui->currentAccessCodeTxt->setEnabled(false);
          ui->currentUseSerial->setChecked(false);
          ui->currentUseSerial->setEnabled(false);
          on_newAccessCodeTxt_editingFinished();
          ui->newAccessCodeTxt->setEnabled(true);
          ui->newUseSerial->setEnabled(true);
          break;
      case CONFIG_PROTECTION_DISABLE:
      case CONFIG_PROTECTION_ENABLED:
          on_currentAccessCodeTxt_editingFinished();
          ui->currentAccessCodeTxt->setEnabled(true);
          ui->currentUseSerial->setEnabled(true);
          ui->newAccessCodeTxt->clear();
          ui->newAccessCodeTxt->setEnabled(false);
          ui->newUseSerial->setChecked(false);
          ui->newUseSerial->setEnabled(false);
          break;
      case CONFIG_PROTECTION_CHANGE:
          on_currentAccessCodeTxt_editingFinished();
          ui->currentAccessCodeTxt->setEnabled(true);
          ui->currentUseSerial->setEnabled(true);
          on_newAccessCodeTxt_editingFinished();
          ui->newAccessCodeTxt->setEnabled(true);
          ui->newUseSerial->setEnabled(true);
          break;
      }

}

void YubiAccBox::on_currentAccessCodeTxt_editingFinished() {
      QString txt = ui->currentAccessCodeTxt->text();
      YubiKeyUtil::qstrClean(&txt, (size_t)ACC_CODE_SIZE * 2);
      ui->currentAccessCodeTxt->setText(txt);
}

void YubiAccBox::on_newAccessCodeTxt_editingFinished() {
      QString txt = ui->newAccessCodeTxt->text();
      YubiKeyUtil::qstrClean(&txt, (size_t)ACC_CODE_SIZE * 2);
      ui->newAccessCodeTxt->setText(txt);
}

bool YubiAccBox::checkConfirm() {
    QSettings settings;
    if(!settings.value(SG_ENABLE_CONF_PROTECTION).toBool() &&
            !YubiKeyLogger::isLogging() &&
            ui->configProtectionCombo->currentIndex() == CONFIG_PROTECTION_ENABLE) {
        //Confirm from client
        ConfirmBox confirm(this);
        confirm.setConfirmIndex(ConfirmBox::Confirm_ConfigurationProtection);
        int ret = confirm.exec();

        switch (ret) {
            case 1:     //Yes
                break;
            default:    //No
                return false;
        }
    }
    return true;
}

QString YubiAccBox::newAccessCode() {
     if(ui->configProtectionCombo->currentIndex() == CONFIG_PROTECTION_DISABLE) {
         return ACCESS_CODE_DEFAULT;
     } else {
         return ui->newAccessCodeTxt->text();
     }
}

QString YubiAccBox::currentAccessCode() {
    return ui->currentAccessCodeTxt->text();
}
