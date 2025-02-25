#include "opera_notes.h"

#include <QtGui>
#include <iostream>
#include <string>
#include <vector>
#include <boost/regex.hpp>

opera_notes::opera_notes(QWidget *parent)
        : QMainWindow(parent) {

    ui.setupUi(this);

    ui.action_open->setIcon(QIcon::fromTheme("document-open"));
    ui.action_save->setIcon(QIcon::fromTheme("document-save"));
    ui.action_save_all->setIcon(QIcon::fromTheme("document-save-all"));
    ui.action_exit->setIcon(QIcon::fromTheme("application-exit"));
    ui.action_reload->setIcon(QIcon::fromTheme("view-refresh"));
    ui.action_about->setIcon(QIcon::fromTheme("help-about"));

    model_ = new QStandardItemModel(this);

    notes_filename_= QDir::homePath() + "/.opera/notes.adr";

    loadSettings();

    parser_ = OperaNotesParser(notes_filename_.toStdString());

    if (QFile::exists(notes_filename_))
        setupModel();
    else
        notes_filename_ = "";

    ui.combo->setModel(model_);

    mapper_ = new QDataWidgetMapper(this);
    mapper_->setModel(model_);
    mapper_->addMapping(ui.text, 1);
    mapper_->setCurrentIndex(0);

    connect(ui.combo, SIGNAL(currentIndexChanged(int)), mapper_, SLOT(setCurrentIndex(int)));
}

opera_notes::~opera_notes() {
    model_->clear();
}

void opera_notes::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui.retranslateUi(this);
            break;
        default:
            break;
    }
}

void opera_notes::closeEvent(QCloseEvent *e) {
    saveSettings();
    QWidget::closeEvent(e);
}

void opera_notes::setupModel() {
    int index = ui.combo->currentIndex();

    model_->clear();
    ui.text->clear();
    ui.combo->clear();

    for (std::size_t i = 0; i < parser_.notes().size(); i++) {
        QList<QStandardItem *> row;

        QString note = QString::fromUtf8(parser_.notes().at(i).note().c_str()).replace("\n", "<br>") + "<br>";
        QString date = QString::fromUtf8(OperaNotesParser::formatUnixTime(parser_.notes().at(i).date()).c_str());
        QString url = QString::fromUtf8(parser_.notes().at(i).url().c_str());
        QString title = QString::fromUtf8(parser_.notes().at(i).title().c_str());

        if (!url.isEmpty()) {
            url = "<a href='" + url + "'>" + url + "</a>";
            note += "<br>" + url + "<br>";
        }

        row << new QStandardItem(date + " - " + title)
            << new QStandardItem(note);

        model_->appendRow(row);
    }

    if (index <= ui.combo->count() && index >= 0)
        ui.combo->setCurrentIndex(index);
    else if (index >= ui.combo->count())
        ui.combo->setCurrentIndex(ui.combo->count() - 1);
}

void opera_notes::on_action_open_triggered() {
    QString path = QDir::homePath();

    if (QDir(path + "/.opera/").exists())
        path += "/.opera/";

    notes_filename_ = QFileDialog::getOpenFileName(this, tr("Select an Opera notes file"),
            path, tr("Opera notes files(notes.adr);;All files(*)"), NULL, QFileDialog::ReadOnly);

    if (!notes_filename_.isEmpty()) {
        parser_= OperaNotesParser(notes_filename_.toStdString());
        setupModel();
    }
}

void opera_notes::on_action_reload_triggered() {
    if (!notes_filename_.isEmpty())
        setupModel();
}

void opera_notes::on_action_save_triggered() {
    int index = ui.combo->currentIndex();
    QString note = QString::fromUtf8(parser_.notes().at(index).note().c_str()).replace("<br>", "\n");
    QString date = QString::fromUtf8(OperaNotesParser::formatUnixTime(parser_.notes().at(index).date()).c_str());
    QString url = QString::fromUtf8(parser_.notes().at(index).url().c_str());
    QString title = QString::fromUtf8(parser_.notes().at(index).title().c_str());

    title = QString::fromUtf8(boost::regex_replace(title.toStdString(), boost::regex("[\\*\\?]"), "").c_str());

    QString filename = QDir::currentPath() + "/" + tr("Opera Note - %1 - %2.txt").arg(date).arg(title);

    filename = QFileDialog::getSaveFileName(this, tr("Save note"),
            filename,
            tr("Text files(*.txt);;All files(*)"), NULL, QFileDialog::ReadOnly);

    if (!filename.isEmpty()) {
        QFile file(filename);
        int result = 0;

        file.open(QIODevice::WriteOnly | QIODevice::Text);
        result = file.write(note.toUtf8() + "\n\n" + url.toUtf8());
        file.close();

        if (result == -1) {
            QMessageBox::warning(this, tr("Error"), tr("An error occurred trying to save \"%1\".").arg(filename));
        } else {
            ui.statusbar->showMessage(tr("File saved."), 30000);
        }

        QDir::setCurrent(QFileInfo(filename).absolutePath());
    }
}

void opera_notes::on_action_save_all_triggered() {
    QString path = "";
    path = QFileDialog::getExistingDirectory(this, tr("Select a folder"), QDir::currentPath(),
            QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);

    if (!path.isEmpty()) {
        if (QMessageBox::question(this, tr("Save All"),
                tr("Files will be saved to \"%1/\".").arg(path)
                + tr("  Existing Files will be overwritten.\n\nProceed?"),
                QMessageBox::Ok | QMessageBox::Cancel)
        == QMessageBox::Ok) {

            bool success = true;

            //for (int i = 0; success && i < model_->rowCount(); i++) {
            for (int i = 0; i < model_->rowCount(); i++) {
                QString note = QString::fromUtf8(parser_.notes().at(i).note().c_str()).replace("<br>", "\n");
                QString date = QString::fromUtf8(OperaNotesParser::formatUnixTime(parser_.notes().at(i).date()).c_str());
                QString url = QString::fromUtf8(parser_.notes().at(i).url().c_str());
                QString title = QString::fromUtf8(parser_.notes().at(i).title().c_str());

                title = QString::fromUtf8(boost::regex_replace(title.toStdString(), boost::regex("[\\*\\?]"), "").c_str());

                QString filename = path + "/" + tr("Opera Note - %1 - %2.txt").arg(date).arg(title.replace(QDir::separator(), "_"));

                QFile file(filename);
                int result = 0;

                file.open(QIODevice::WriteOnly | QIODevice::Text);
                result = file.write(note.toUtf8() + "\n\n" + url.toUtf8());
                file.close();

                if (result == -1) {
                    QMessageBox::warning(this, tr("Error"), tr("An error occurred trying to save \"%1\".").arg(filename));
                    success = false;
                }
            }

            if (success)
                ui.statusbar->showMessage(tr("Files saved."), 30000);
            else
                ui.statusbar->showMessage(tr("Files saved (with errors)."), 30000);
        }

        QDir::setCurrent(path);
    }
}

void opera_notes::on_action_about_triggered() {
    QMessageBox::about(this, tr("About"),
            tr("Opera Notes Reader\nversion %1").arg(APP_VERSION));
}

void opera_notes::loadSettings() {
    QSettings settings;
    resize(settings.value("size", this->size()).toSize());
    QDir::setCurrent(settings.value("save_path", QDir::currentPath()).toString());
    notes_filename_ = settings.value("notes_filename", notes_filename_).toString();
}

void opera_notes::saveSettings() {
    QSettings settings;
    settings.setValue("size", size());
    settings.setValue("save_path", QDir::currentPath());
    settings.setValue("notes_filename", notes_filename_);
}
