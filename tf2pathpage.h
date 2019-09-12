#ifndef TF2PATHPAGE_H
#define TF2PATHPAGE_H

#include <QWizardPage>
#include <QComboBox>

class TF2PathPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit TF2PathPage(QWidget *parent = nullptr);
    bool isComplete() const override;
    void initializePage() override;
private slots:
    void browse();
private:
    QComboBox *cbDirectory;
};

#endif // TF2PATHPAGE_H
