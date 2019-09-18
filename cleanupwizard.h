#ifndef CLEANUPWIZARD_H
#define CLEANUPWIZARD_H

#include <QWizard>

class CleanupWizard : public QWizard
{
    Q_OBJECT
public:
    explicit CleanupWizard(QWidget *parent = nullptr);
    void accept() override;
};

#endif // CLEANUPWIZARD_H
