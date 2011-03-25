/*
 * MeeGo Content Aggregation - Email Plugin
 * Copyright © 2010, Intel Corporation.
 */

#ifndef __emailfeedmodel_h
#define __emailfeedmodel_h

#include <QObject>
#include <QList>
#include <QDateTime>

#include <feedmodel.h>
#include <actions.h>

struct EmailMessage {
    QString contact;
    QString preview;
    QString id;
    QDateTime timestamp;
};

class EmailMessageListModel;

class EmailFeedModel: public McaFeedModel
{
    Q_OBJECT

public:
    EmailFeedModel(QVariant account, QObject *parent = 0);
    ~EmailFeedModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);

protected slots:
    void sourceRowsInserted(const QModelIndex& parent, int first, int last);
    void sourceRowsRemoved(const QModelIndex& parent, int first, int last);
    void sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void resetModel();
    void performAction(QString uniqueid, QString action);

protected:
    void insertRows(int first, int last);
    void readRow(EmailMessage *message, int row);

private:
    EmailMessageListModel *m_source;
    QList<EmailMessage *> m_messages;
    McaActions *m_actions;
};

#endif  // __emailfeedmodel_h
