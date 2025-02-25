#ifndef OPERA_NOTES_H
#define OPERA_NOTES_H

#include "ui_opera_notes.h"
#include "version.h"
#include "opera_notes_parser.h"

#include <QtGui/QMainWindow>
#include <QStandardItemModel>
#include <QDataWidgetMapper>

class opera_notes : public QMainWindow
{
    Q_OBJECT

public:
    opera_notes(QWidget *parent = 0);
    ~opera_notes();

private:
    Ui::opera_notesClass ui;
    QStandardItemModel *model_;
    QDataWidgetMapper *mapper_;
    QString notes_filename_;
    OperaNotesParser parser_;

    void setupModel();
    void saveSettings();
    void loadSettings();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);

private slots:
    void on_action_open_triggered();
    void on_action_reload_triggered();
    void on_action_save_triggered();
    void on_action_save_all_triggered();
    void on_action_about_triggered();
};
#endif // OPERA_NOTES_H
