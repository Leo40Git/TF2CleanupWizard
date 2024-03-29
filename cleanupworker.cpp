#include "cleanupworker.h"

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QCryptographicHash>
#include <QHash>
#include <QDesktopServices>
#include <QUrl>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "tlhelp32.h"
#include "psapi.h"

void CleanupWorker::doWork(const QString &pathSteam, const QString &pathTF2, quint64 steamID, const QString &backupFolderName)
{
    this->pathSteam = pathSteam;
    this->pathTF2 = pathTF2;
    this->steamID = steamID;
    this->backupFolderName = backupFolderName;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CleanupWorker::timerFire);
    emit log(" == REMOVING FILES AND VALIDATING == ");
    deleteFilesAndValidate();
}

void CleanupWorker::doWork2()
{
    emit log(" == CLEARING STEAM CLOUD DATA == ");
    blankRemoteData();
    emit log(" == LAUNCHING WITH CLEANUP OPTIONS == ");
    launchWithCleanOptions();
    emit log("done! enjoy your squeaky clean TF2 install!");
    emit done();
}

HWND hValidWnd = nullptr;

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM param) {
    (void)param;
    QByteArray nameBuf;
    nameBuf.clear();
    int len = GetWindowTextLengthA(hwnd);
    if (len == 0)
        return TRUE;
    nameBuf.resize(len + 1);
    GetWindowTextA(hwnd, nameBuf.data(), len + 1);
    if (QString::fromLocal8Bit(nameBuf).startsWith("Validating Steam files - ")) {
        hValidWnd = hwnd;
        return FALSE;
    }
    return TRUE;
}

void CleanupWorker::deleteFilesAndValidate()
{
    QStringList entryList;
    int progress;
    QDir dir_tf(QString("%1/tf").arg(pathTF2));
    QDir dir_custom(QString("%1/tf/custom").arg(pathTF2));
    if (dir_custom.exists()) {
        // backup and remove tf/custom
        QDir dir_custom_backup(QString("%1/tf/custom_backup/%2").arg(pathTF2).arg(backupFolderName));
        emit log(QString("- backing up files from \"%1\" -").arg(dir_custom.absolutePath()));
        if (dir_custom_backup.exists())
            dir_custom_backup.removeRecursively();
        emit log(QString("backing up to \"%1\"").arg(dir_custom_backup.absolutePath()));
        dir_custom_backup.mkpath(".");
        entryList = dir_custom.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        emit setProgressLabel("Backing up custom assets - subdirectories");
        emit setProgressFormat("%v/%m");
        emit setProgressMaximum(entryList.size());
        emit setProgressValue((progress = 0));
        foreach (QString subdirName, entryList) {
            if (QString::compare("workshop", subdirName) == 0) {
                emit log(QString("ignoring subdirectory \"workshop\""));
                emit setProgressValue(++progress);
                continue;
            }
            QString soundCacheFile = QString("%1/sound/sound.cache").arg(subdirName);
            if (dir_custom.exists(soundCacheFile)) {
                emit log(QString("removing sound cache file \"%1\"").arg(soundCacheFile));
                dir_custom.cd((QString("%1/sound").arg(subdirName)));
                dir_custom.removeRecursively();
                dir_custom.cd("../..");
            }
            emit log(QString("backing up subdirectory \"%1\"").arg(subdirName));
            dir_tf.rename(QString("%1/%2").arg(dir_custom.path()).arg(subdirName), QString("%1/%2").arg(dir_custom_backup.path()).arg(subdirName));
            emit setProgressValue(++progress);
        }
        QString custom_readme_hash = "063bd9b29a9c10f4a59e5eacb00fc8a9c20fc3d756194b78ca9e163568916715";
        entryList = dir_custom.entryList(QDir::Files);
        emit setProgressLabel("Backing up custom assets - files");
        emit setProgressFormat("%v/%m");
        emit setProgressMaximum(entryList.size());
        emit setProgressValue((progress = 0));
        foreach (QString fileName, entryList) {
            if (QString::compare("readme.txt", fileName) == 0) {
                QFile file(dir_custom.absoluteFilePath(fileName));
                file.open(QFile::ReadOnly);
                QByteArray data = file.readAll();
                file.close();
                QString hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
                if (custom_readme_hash.compare(hash, Qt::CaseInsensitive) == 0) {
                    emit log(QString("ignoring distributed file \"readme.txt\""));
                    emit setProgressValue(++progress);
                    continue;
                }
            }
            if (fileName.endsWith(".sound.cache")) {
                emit log(QString("ignoring sound cache file \"%1\"").arg(fileName));
                emit setProgressValue(++progress);
                continue;
            }
            emit log(QString("backing up file \"%1\"").arg(fileName));
            dir_tf.rename(QString("%1/%2").arg(dir_custom.path()).arg(fileName), QString("%1/%2").arg(dir_custom_backup.path()).arg(fileName));
            emit setProgressValue(++progress);
        }
        emit log(QString("removing directory \"%1\"").arg(dir_custom.absolutePath()));
        dir_custom.removeRecursively();
    } else
        emit log(QString("directory \"%1\" does not exist").arg(dir_custom.absolutePath()));
    QDir dir_cfg(QString("%1/tf/cfg").arg(pathTF2));
    if (dir_cfg.exists()) {
        // load tf/cfg hashes
        QHash<QString, QByteArray> cfgHashes;
        QFile cfgHashesFile(":/cfg_hashes.txt");
        cfgHashesFile.open(QFile::ReadOnly | QFile::Text);
        while (!cfgHashesFile.atEnd()) {
            QList<QByteArray> parts = cfgHashesFile.readLine().chopped(1).split(',');
            cfgHashes[parts[0]] = parts[1];
        }
        cfgHashesFile.close();
        // backup and remove tf/cfg
        QDir dir_cfg_backup(QString("%1/tf/cfg_backup/%2").arg(pathTF2).arg(backupFolderName));
        emit log(QString("- backing up files from \"%1\" -").arg(dir_cfg.absolutePath()));
        if (dir_cfg_backup.exists())
            dir_cfg_backup.removeRecursively();
        emit log(QString("backing up to \"%1\"").arg(dir_cfg_backup.absolutePath()));
        dir_cfg_backup.mkpath(".");
        entryList = dir_cfg.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        emit setProgressLabel("Backing up configuration scripts - subdirectories");
        emit setProgressFormat("%v/%m");
        emit setProgressMaximum(entryList.size());
        emit setProgressValue((progress = 0));
        foreach (QString subdirName, entryList) {
            emit log(QString("backing up subdirectory \"%1\"").arg(subdirName));
            dir_tf.rename(QString("%1/%2").arg(dir_cfg.path()).arg(subdirName), QString("%1/%2").arg(dir_cfg_backup.path()).arg(subdirName));
            emit setProgressValue(++progress);
        }
        entryList = dir_cfg.entryList(QDir::Files);
        emit setProgressLabel("Backing up configuration scripts - files");
        emit setProgressFormat("%v/%m");
        emit setProgressMaximum(entryList.size());
        emit setProgressValue((progress = 0));
        foreach (QString fileName, entryList) {
            if (fileName.endsWith(".scr")) {
                emit log(QString("ignoring automatically generated file \"%1\"").arg(fileName));
                emit setProgressValue(++progress);
                continue;
            }
            if (cfgHashes.contains(fileName)) {
                if (cfgHashes[fileName].compare("$ignore", Qt::CaseInsensitive) == 0) {
                    emit log(QString("ignoring distributed file \"%1\"").arg(fileName));
                    emit setProgressValue(++progress);
                    continue;
                }
                QFile file(dir_cfg.absoluteFilePath(fileName));
                file.open(QFile::ReadOnly);
                QByteArray data = file.readAll();
                file.close();
                QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
                if (cfgHashes[fileName].compare(hash, Qt::CaseInsensitive) == 0) {
                    emit log(QString("ignoring distributed file \"%1\"").arg(fileName));
                    emit setProgressValue(++progress);
                    continue;
                }
            }
            emit log(QString("backing up file \"%1\"").arg(fileName));
            dir_tf.rename(QString("%1/%2").arg(dir_cfg.path()).arg(fileName), QString("%1/%2").arg(dir_cfg_backup.path()).arg(fileName));
            emit setProgressValue(++progress);
        }
        emit log(QString("removing directory \"%1\"").arg(dir_cfg.absolutePath()));
        dir_cfg.removeRecursively();
    } else
        emit log(QString("directory \"%1\" does not exist").arg(dir_cfg.absolutePath()));
    emit log("- revalidating TF2 installation -");
    if (!QDesktopServices::openUrl(QUrl("steam://validate/440"))) {
        emit log("failed to start validation - could not open link \"steam://validate/440\"");
        emit log("please validate your game manually later");
        emit setProgressLabel("...");
        emit setProgressFormat("%v/%m");
        emit setProgressMaximum(0);
        emit setProgressValue(-1);
        doWork2();
    }
    emit log("waiting for validation progress window to open");
    deadline = new QDeadlineTimer(30000); // 30 seconds = half a minute
    while (!hValidWnd) {
        EnumWindows(EnumWindowsCallback, 0);
        if (deadline->hasExpired()) {
            emit log("failed to start validation - timed out (validation window took >30 seconds to open");
            emit log("please validate TF2 manually later");
            emit setProgressLabel("...");
            emit setProgressFormat("%v/%m");
            emit setProgressMaximum(0);
            emit setProgressValue(-1);
            doWork2();
            break;
        }
    }
    emit log("TF2 is now validating, please wait...");
    emit setProgressLabel("Validating Steam files");
    emit setProgressFormat("%p%");
    emit setProgressMaximum(100);
    emit setProgressValue(0);
    timer->setInterval(500);
    timer->start();
    deadline = new QDeadlineTimer(30000); // 30 seconds = half a minute
}

void CleanupWorker::timerFire()
{
    QByteArray nameBuf;
    nameBuf.clear();
    int len = GetWindowTextLengthA(hValidWnd);
    if (len <= 25)
        return;
    nameBuf.resize(len + 1);
    GetWindowTextA(hValidWnd, nameBuf.data(), len + 1);
    int percent = QString::fromLocal8Bit(nameBuf).mid(25).chopped(10).toInt();
    if (deadline) {
        if (percent == 0) {
            if (deadline->hasExpired()) {
                emit log("failed to start validation - timed out (progress was at 0% for >30 seconds");
                emit log("please cancel the validation process, and manually validate TF2 later");
                emit setProgressLabel("...");
                emit setProgressFormat("%v/%m");
                emit setProgressMaximum(0);
                emit setProgressValue(-1);
                doWork2();
                return;
            }
        } else {
            delete deadline;
            deadline = nullptr;
        }
    }
    emit setProgressValue(percent);
    if (percent == 100) {
        timer->stop();
        delete timer;
        timer = nullptr;
        Sleep(3000);
        CloseWindow(hValidWnd);
        emit log("TF2 validated successfully");
        emit setProgressLabel("...");
        emit setProgressFormat("%v/%m");
        emit setProgressMaximum(0);
        emit setProgressValue(-1);
        doWork2();
        return;
    }
}

void CleanupWorker::blankRemoteData()
{
    QDir dir_userdata(QString("%1/userdata").arg(pathSteam));
    // see https://developer.valvesoftware.com/wiki/SteamID
    QString accountID = QString::number(((steamID >> 1) & 0x7FFFFFFF) * 2 + (steamID & 1));
    dir_userdata.cd(accountID);
    if (!dir_userdata.exists()) {
        emit log(QString("directory \"%1\" does not exist").arg(dir_userdata.absolutePath()));
        return;
    }
    dir_userdata.cd("440");
    if (!dir_userdata.exists()) {
        emit log(QString("directory \"%1\" does not exist").arg(dir_userdata.absolutePath()));
        return;
    }
    dir_userdata.cd("remote");
    if (!dir_userdata.exists()) {
        emit log(QString("directory \"%1\" does not exist").arg(dir_userdata.absolutePath()));
        return;
    }
    dir_userdata.cd("cfg");
    if (!dir_userdata.exists()) {
        emit log(QString("directory \"%1\" does not exist").arg(dir_userdata.absolutePath()));
        return;
    }
    int progress;
    QStringList entryList = dir_userdata.entryList(QDir::Files);
    emit setProgressFormat("%v/%m");
    emit setProgressMaximum(entryList.size());
    emit log(QString(" - backing up Steam Cloud files in \"%1\" -").arg(dir_userdata.absolutePath()));
    QDir dir_userdata_backup(QString("%1/../cfg_backup/%2").arg(dir_userdata.absolutePath()).arg(backupFolderName));
    if (dir_userdata_backup.exists())
        dir_userdata_backup.removeRecursively();
    dir_userdata_backup.mkpath(".");
    emit log(QString("backing up to \"%2\"").arg(dir_userdata_backup.absolutePath()));
    emit setProgressLabel("Backing up configuration files");
    emit setProgressValue((progress = 0));
    foreach (QString fileName, entryList) {
        QFile file(dir_userdata.absoluteFilePath(fileName));
        emit log(QString("backing up file \"%1\"").arg(fileName));
        file.copy(QString("%1/%2").arg(dir_userdata_backup.absolutePath()).arg(fileName));
        emit setProgressValue(++progress);
    }
    emit log(QString("clearing Steam Cloud files in \"%1\"").arg(dir_userdata.absolutePath()));
    emit setProgressLabel("Clearing Steam Cloud configuration files");
    emit setProgressValue((progress = 0));
    foreach (QString fileName, entryList) {
        QFile file(dir_userdata.absoluteFilePath(fileName));
        emit log(QString("clearing file \"%1\"").arg(fileName));
        file.open(QFile::WriteOnly | QFile::Truncate);
        file.close();
        emit setProgressValue(++progress);
    }
    emit setProgressLabel("...");
    emit setProgressFormat("%v/%m");
    emit setProgressMaximum(0);
    emit setProgressValue(-1);
}

void CleanupWorker::launchWithCleanOptions()
{
    emit log("launching TF2 with the following launch options: \"-novid -autoconfig +host_writeconfig +mat_savechanges +quit\"");
    if (!QDesktopServices::openUrl(QUrl("steam://run/440//-novid%20-autoconfig%20%2Bhost_writeconfig%20%2Bmat_savechanges%20%2Bquit"))) {
        emit log("failed to open TF2 - could not open link \"steam://run/440//-novid%20-autoconfig%20%2Bhost_writeconfig%20%2Bmat_savechanges%20%2Bquit\"");
        emit log("please run TF2 manually with the above launch options");
        return;
    }
    emit log("waiting for TF2 to close");

    deadline = new QDeadlineTimer(30000); // 30 seconds = half a minute

    // TIME TO SEARCH FOR THE GAME EXE, YAY
    HANDLE hGameProc = nullptr;
    bool match = false;

    while (!match) {
        if (deadline->hasExpired()) {
            emit log("failed while looking for TF2 executable (hl2.exe) - timed out (couldn't find executable for >30 seconds");
            break;
        }

        // setup to loop through process snapshot
        HANDLE hSnap = nullptr;
        PROCESSENTRY32 pe32;
        ZeroMemory(&pe32, sizeof(pe32));
        pe32.dwSize = sizeof(pe32);

        // take that snapshot
        hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (!hSnap) {
            DWORD err = GetLastError();
            emit log(QString("failed while looking for TF2 executable (hl2.exe) - CreateToolhelp32Snapshot, errcode 0x%1").arg(QString::number(err, 16).toUpper()));
            break;
        }

        // loop through and grab the matching EXE
        if (Process32First(hSnap, &pe32)) {
            do {
                hGameProc = OpenProcess(PROCESS_QUERY_INFORMATION, true, pe32.th32ProcessID);
                // if we can't open the process, continue
                if (!hGameProc)
                    continue;
                CloseHandle(hGameProc);
                // if the EXE names don't match, continue
#ifdef UNICODE
                QString str = QString::fromWCharArray(pe32.szExeFile);
#else
                QString str = QString::fromLocal8Bit(pe32.szExeFile);
#endif
                if (QString("hl2.exe").compare(str) != 0)
                    continue;
                // we got a match! open the process and break this loop
                hGameProc = OpenProcess(PROCESS_QUERY_INFORMATION, true, pe32.th32ProcessID);
                match = true;
                break;
            } while (Process32Next(hSnap, &pe32));
        }

        CloseHandle(hSnap);
    }

    delete deadline;
    deadline = nullptr;

    if (match) {
        DWORD exitCode = STILL_ACTIVE;
        while (exitCode == STILL_ACTIVE)
            GetExitCodeProcess(hGameProc, &exitCode);
        CloseHandle(hGameProc);
        emit log("TF2 has closed");
    }
}
