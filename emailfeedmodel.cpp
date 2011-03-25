/*
 * MeeGo Content Aggregation - Email Plugin
 * Copyright © 2010, Intel Corporation.
 */

#include "emailfeedmodel.h"

#include <QDebug>

#include <QDateTime>
#include <QStringList>
#include <QDesktopServices>

#include <actions.h>

#include <emailmessagelistmodel.h>

const int FetchBatch = 10;

// TODO: firm up this decision and move to a more central place
const int BodyPreviewSize = 160;  // big enough for an SMS, larger than a tweet

EmailFeedModel::EmailFeedModel(QVariant account, QObject *parent):
        McaFeedModel(parent)
{
    // TODO: pass the account id when Carl makes that available
    m_source = new EmailMessageListModel(this);
    m_source->setAccountKey(account);
    m_actions = new McaActions;

    connect(m_source, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsInserted(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));
    connect(m_source, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(sourceDataChanged(QModelIndex,QModelIndex)));
    connect(m_source, SIGNAL(modelReset()),
            this, SLOT(resetModel()));
    connect(m_actions, SIGNAL(standardAction(QString,QString)),
            this, SLOT(performAction(QString,QString)));

    int count = m_source->rowCount();
    if (count > FetchBatch)
        count = FetchBatch;
    insertRows(0, count - 1);
}

EmailFeedModel::~EmailFeedModel()
{
    delete m_source;
}

//
// public member functions
//

int EmailFeedModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_messages.count();
}

QVariant EmailFeedModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row >= m_messages.count())
        return QVariant();

    EmailMessage *message = m_messages.at(row);

    switch (role) {
    case RequiredTypeRole:
        return "content";

    case RequiredUniqueIdRole:
        return message->id;

    case RequiredTimestampRole:
        return message->timestamp;

    case GenericTitleRole:
        return message->contact;

    case GenericContentRole:
        return message->preview;

    case CommonActionsRole:
        return QVariant::fromValue<McaActions*>(m_actions);

    default:
        return QVariant();
    }

    return QVariant();
}

bool EmailFeedModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return (m_messages.count() < m_source->rowCount()) || m_source->canFetchMore(QModelIndex());
}

void EmailFeedModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent)
    int first = m_messages.count();
    int count = m_source->rowCount() - first;
    if (count > FetchBatch)
        count = FetchBatch;
    if (count > 0)
        sourceRowsInserted(QModelIndex(), first, first + count - 1);
    else if (m_source->canFetchMore(QModelIndex()))
        m_source->fetchMore(QModelIndex());
}

//
// protected slots
//

void EmailFeedModel::sourceRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(parent)
    beginInsertRows(QModelIndex(), first, last);
    insertRows(first, last);
    endInsertRows();
}

void EmailFeedModel::sourceRowsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(parent)
    beginRemoveRows(QModelIndex(), first, last);
    for (int i = first; i <= last; i++) {
        delete m_messages.at(i);
        m_messages.removeAt(i);
    }
    endRemoveRows();
}

void EmailFeedModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    int first = topLeft.row();
    int last = bottomRight.row();
    for (int i = first; i <= last; i++)
        readRow(m_messages.at(i), i);
    emit dataChanged(index(first), index(last));
}

void EmailFeedModel::resetModel()
{
    beginResetModel();
    foreach (EmailMessage *message, m_messages)
        delete message;
    m_messages.clear();
    insertRows(0, m_source->rowCount() - 1);
    endResetModel();
}

void EmailFeedModel::performAction(QString action, QString uniqueid)
{
    qDebug() << "Action" << action << "called for email item" << uniqueid;

    if (action == "default") {
        QString executable("meego-tablet-wrapper");
        QStringList parameters;
        parameters << "--app" << "meego-tablet-email";
        parameters << "--opengl" << "--fullscreen";
        parameters << "--cmd" << "openReader";
        parameters << "--cdata" << uniqueid;
        QProcess::startDetached(executable, parameters);
    }
}

//
// protected member functions
//

void EmailFeedModel::insertRows(int first, int last)
{
    for (int i = last; i >= first; i--) {
        EmailMessage *message = new EmailMessage;
        readRow(message, i);
        m_messages.insert(first, message);
    }
}

void EmailFeedModel::readRow(EmailMessage *message, int row)
{
    QModelIndex sourceIndex = m_source->index(row);

    // prefer contact name to email address
    message->contact = m_source->data(sourceIndex, EmailMessageListModel::MessageSenderDisplayNameRole).toString();
    if (message->contact.isEmpty())
        message->contact = m_source->data(sourceIndex, EmailMessageListModel::MessageSenderEmailAddressRole).toString();

    // TODO: handle HTML mail more intelligently somehow, for now body is empty for HTML
    message->preview = m_source->data(sourceIndex, QMailMessageModelBase::MessageSubjectTextRole).toString();
    if (message->preview.isEmpty()) {
        // TODO: use Carl's body preview role when he adds that
        QString body = m_source->data(sourceIndex, QMailMessageModelBase::MessageBodyTextRole).toString();
        message->preview = body.left(BodyPreviewSize);
    }

    // TODO: wireframe apparently has an icon, will need to look that up in QtContacts

    message->id = m_source->data(sourceIndex, EmailMessageListModel::MessageUuidRole).toString();
    message->timestamp = m_source->data(sourceIndex, EmailMessageListModel::MessageTimeStampRole).toDateTime();
}
