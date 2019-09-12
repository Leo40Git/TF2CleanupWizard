#ifndef STEAMPATHPAGE_H
#define STEAMPATHPAGE_H

#include <QWizardPage>
#include <QComboBox>

class SteamPathPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SteamPathPage(QWidget *parent = nullptr);
    bool isComplete() const override;
    void initializePage() override;
private slots:
    void browse();
private:
    QComboBox *cbDirectory;
};

#endif // STEAMPATHPAGE_H
