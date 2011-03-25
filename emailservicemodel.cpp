/*
 * MeeGo Content Aggregation - Email Plugin
 * Copyright © 2010, Intel Corporation.
 */

#include <QDebug>
#include <QString>

#include <actions.h>

#include "emailservicemodel.h"

EmailServiceModel::EmailServiceModel(QObject *parent):
        McaServiceModel(parent)
{
    m_accounts = new EmailAccountListModel;
    m_lastId = 0;

    connect(m_accounts, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsInserted(QModelIndex,int,int)));
    connect(m_accounts, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));
    connect(m_accounts, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(sourceDataChanged(QModelIndex,QModelIndex)));
    connect(m_accounts, SIGNAL(modelReset()),
            this, SLOT(resetModel()));

    insertRows(0, m_accounts->rowCount() - 1);
}

EmailServiceModel::~EmailServiceModel()
{
    delete m_accounts;
}

//
// public member functions
//

int EmailServiceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    qDebug() << "EmailServiceModel::rowCount =" << m_services.count();
    return m_services.count();
}

QVariant EmailServiceModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row >= m_services.count())
        return QVariant();

    qDebug() << "EmailServiceModel::data role=" << role;
    switch (role) {
    case CommonDisplayNameRole:
        return m_services.at(row).displayName;

    case RequiredCategoryRole:
        return "email";

    case RequiredNameRole:
        return m_services.at(row).id;

    case CommonConfigErrorRole:
        // assuming we will only show properly configured accounts for now
        return false;

    case CommonActionsRole:
        // until we start sending "true" for CommonConfigErrorRole, not needed
    default:
        qWarning() << "Unhandled data role requested!";
    case CommonIconUrlRole:
        // expect panel to have no icon header for email, or supply it itself
        return QVariant();
    }
}

QVariant EmailServiceModel::accountId(const QString &serviceId)
{
    foreach (EmailService service, m_services) {
        if (service.id == serviceId)
            return service.accountId;
    }

    qWarning() << "No account id found for service id " << serviceId << "in email plugin";
    return QVariant();
}

//
// protected slots
//

void EmailServiceModel::sourceRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(parent)
    beginInsertRows(QModelIndex(), first, last);
    insertRows(first, last);
    endInsertRows();
}

void EmailServiceModel::sourceRowsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(parent)
    beginRemoveRows(QModelIndex(), first, last);
    for (int i = first; i <= last; i++)
        m_services.removeAt(i);
    endRemoveRows();
}

void EmailServiceModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    int first = topLeft.row();
    int last = bottomRight.row();
    for (int i = first; i <= last; i++) {
        EmailService service;
        readRow(&service, i);
        m_services.replace(i, service);
    }
    emit dataChanged(index(first), index(last));
}

void EmailServiceModel::resetModel()
{
    beginResetModel();
    m_services.clear();
    insertRows(0, m_accounts->rowCount() - 1);
    endResetModel();
}

//
// protected member functions
//

void EmailServiceModel::insertRows(int first, int last)
{
    for (int i = last; i >= first; i--) {
        EmailService service;
        readRow(&service, i);
        m_services.insert(first, service);
    }
}

void EmailServiceModel::readRow(EmailService *service, int row)
{
    QModelIndex sourceIndex = m_accounts->index(row);
    service->displayName = m_accounts->data(sourceIndex, EmailAccountListModel::DisplayName).toString();
    service->id = m_accounts->data(sourceIndex, EmailAccountListModel::EmailAddress).toString();
    service->accountId = m_accounts->data(sourceIndex, EmailAccountListModel::MailAccountId);
}
