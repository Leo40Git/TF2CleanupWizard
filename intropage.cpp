#include "intropage.h"

#include <QLabel>
#include <QVBoxLayout>

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Introduction");
    setSubTitle(QString("Welcome to TF2 Cleanup Wizard v%1!").arg(APP_VERSION));

    QLabel *label = new QLabel("This wizard will reset your installation of Team Fortress 2 "
                       "to its default configuration, as if you just installed the "
                       "game for the very first time.<br><br>"
                       "<b>Please note</b> that after this wizard is finished, "
                       "<i>all</i> of your settings will be reset to their default values, including Steam Cloud data, "
                       "and <i>all</i> custom assets and configuration scripts you have installed in "
                       "<code>TF2_FOLDER/tf/custom</code> and <code>TF2_FOLDER/tf/cfg</code> will be removed.<br>"
                       "They will be moved to <code>TF2_FOLDER/tf/custom_backup</code> and "
                       "<code>TF2_FOLDER/tf/cfg_backup</code>, respectively. (Note that <i>the previous contents of these backup directories will be deleted.</i>)<br><br>"
                       "<b>Please make sure Steam is running before continuing.", this);
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    setLayout(layout);
}
