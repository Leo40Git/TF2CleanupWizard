#ifndef INTROPAGE_H
#define INTROPAGE_H

#include <QWizardPage>
#include <QLabel>

class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit IntroPage(QWidget *parent = nullptr);
    void initializePage() override;
private:
    QLabel *label;
};

#endif // INTROPAGE_H
