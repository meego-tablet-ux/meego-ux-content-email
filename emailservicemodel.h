/*
 * MeeGo Content Aggregation - Email Plugin
 * Copyright © 2010, Intel Corporation.
 */

#ifndef __emailservicemodel_h
#define __emailservicemodel_h

#include <QList>

#include <servicemodel.h>
#include <emailaccountlistmodel.h>

struct EmailService {
    QString displayName;
    QString id;
    QVariant accountId;
};

class EmailServiceModel: public McaServiceModel
{
    Q_OBJECT

public:
    EmailServiceModel(QObject *parent = NULL);
    ~EmailServiceModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QVariant accountId(const QString &serviceId);

protected slots:
    void sourceRowsInserted(const QModelIndex& parent, int first, int last);
    void sourceRowsRemoved(const QModelIndex& parent, int first, int last);
    void sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void resetModel();

protected:
    void insertRows(int first, int last);
    void readRow(EmailService *service, int row);

private:
    void configure(QString serviceName);

    EmailAccountListModel *m_accounts;
    QList<EmailService> m_services;
    int m_lastId;
};

#endif  // __emailservicemodel_h
