#include "warningpage.h"

#include <QLabel>
#include <QVBoxLayout>

WarningPage::WarningPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle("Warning!");

    QLabel *label = new QLabel("<b>The wizard will now perform the cleanup operations.</b>"
                               "<h3>The wizard's effects cannot be reversed from this point on!</h3>", this);
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    setLayout(layout);
}
