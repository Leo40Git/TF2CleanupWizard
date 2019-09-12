#include "intropage.h"

#include <QVBoxLayout>

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Introduction");

    label = new QLabel("This wizard will reset your installation of Team Fortress 2 "
                       "to its default configuration, as if you just installed the "
                       "game for the very first time.<br><br>"
                       "This wizard is intended for use before installing some sort of "
                       "performance/quality config, to reset any and all properties "
                       "that a previous config may have changed.<br><br>"
                       "<b>Please note</b> that after this wizard is finished, "
                       "<i>all</i> custom assets and configs you have installed will be removed.<br>"
                       "They will be moved to <code>TF2_FOLDER/tf/custom_backup</code> and "
                       "<code>TF2_FOLDER/tf/cfg_backup</code>, respectively.");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}
