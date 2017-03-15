/*
 * Copyright (c) 2014, Airbitz, Inc.
 * All rights reserved.
 *
 * See the LICENSE file for more information.
 */

#include "LoginJson.hpp"
#include "LoginPackages.hpp"
#include "../Login.hpp"
#include "../LoginPin2.hpp"
#include "../LoginRecovery2.hpp"

namespace abcd {

Status
LoginReplyJson::save(const Login &login)
{
    CarePackage carePackage;
    LoginPackage loginPackage;

    // Password:
    if (passwordAuthBox().ok())
        ABC_CHECK(loginPackage.passwordAuthBoxSet(passwordAuthBox()));
    if (passwordBox().ok())
        ABC_CHECK(loginPackage.passwordBoxSet(passwordBox()));
    if (passwordKeySnrp().ok())
        ABC_CHECK(carePackage.passwordKeySnrpSet(passwordKeySnrp()));

    // Recovery v1:
    if (questionBox().ok())
        ABC_CHECK(carePackage.questionBoxSet(questionBox()));
    if (questionKeySnrp().ok())
        ABC_CHECK(carePackage.questionKeySnrpSet(questionKeySnrp()));
    if (recoveryBox().ok())
        ABC_CHECK(loginPackage.recoveryBoxSet(recoveryBox()));
    if (recoveryKeySnrp().ok())
        ABC_CHECK(carePackage.recoveryKeySnrpSet(recoveryKeySnrp()));

    // Keys:
    if (rootKeyBox().ok())
        ABC_CHECK(rootKeyBox().save(login.paths.rootKeyPath()));
    if (syncKeyBox().ok())
        ABC_CHECK(loginPackage.syncKeyBoxSet(syncKeyBox()));
    if (repos().ok())
        ABC_CHECK(repos().save(login.paths.reposPath()));

    // Keys to save unencrypted:
    DataChunk pin2Key;
    if (pin2KeyBox().decrypt(pin2Key, login.dataKey()))
        ABC_CHECK(loginPin2KeySave(pin2Key, login.paths));

    DataChunk recovery2Key;
    if (recovery2KeyBox().decrypt(recovery2Key, login.dataKey()))
        ABC_CHECK(loginRecovery2KeySave(recovery2Key, login.paths));

    // Write to disk:
    ABC_CHECK(carePackage.save(login.paths.carePackagePath()));
    ABC_CHECK(loginPackage.save(login.paths.loginPackagePath()));

    return Status();
}

} // namespace abcd