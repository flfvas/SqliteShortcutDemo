#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. 简单的界面布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    label = new QLabel("当前 Xuhao: 1", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 20px;");
    layout->addWidget(label);
    setCentralWidget(centralWidget);
    resize(300, 200);

    // 2. 初始化数据库
    initDatabase();
    loadData();

    // 3. 设置 Ctrl+Q 快捷键
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
    connect(shortcut, &QShortcut::activated, this, &MainWindow::incrementXuhao);
}

MainWindow::~MainWindow()
{
    db.close();
}

void MainWindow::initDatabase()
{
    // 创建或打开 SQLite 数据库文件
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");

    if (!db.open()) {
        qDebug() << "无法打开数据库:" << db.lastError().text();
        return;
    }

    // 创建表（如果不存在）
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY, val INTEGER)");
}

void MainWindow::loadData()
{
    QSqlQuery query("SELECT val FROM settings WHERE id = 1");
    if (query.next()) {
        Xuhao = query.value(0).toInt();
    } else {
        // 如果表是空的，插入初始值 1
        QSqlQuery insert;
        insert.prepare("INSERT INTO settings (id, val) VALUES (1, ?)");
        insert.addBindValue(Xuhao);
        insert.exec();
    }
    label->setText(QString("当前 Xuhao: %1").arg(Xuhao));
}

void MainWindow::incrementXuhao()
{
    Xuhao++;
    label->setText(QString("当前 Xuhao: %1").arg(Xuhao));
    updateDatabase();
}

void MainWindow::updateDatabase()
{
    QSqlQuery query;
    query.prepare("UPDATE settings SET val = ? WHERE id = 1");
    query.addBindValue(Xuhao);
    if (!query.exec()) {
        qDebug() << "更新失败:" << query.lastError().text();
    }
}
