#ifndef INTROPAGE_H
#define INTROPAGE_H

#include <QWizardPage>

class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit IntroPage(QWidget *parent = nullptr);
};

#endif // INTROPAGE_H
