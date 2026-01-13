#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- 第一部分：快捷键与核心连接 ---
    setupShortcuts();

    // --- 第二部分：各功能模块化初始化 ---
    setupUI();
    initDatabase();
    loadData();
}

MainWindow::~MainWindow()
{
    db.close();
}

// 分离功能：设置快捷键
void MainWindow::setupShortcuts()
{
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
    connect(shortcut, &QShortcut::activated, this, &MainWindow::incrementXuhao);
}

// 分离功能：界面初始化
void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");

    layout->addWidget(label);
    setCentralWidget(centralWidget);
    resize(300, 200);
}

// 分离功能：数据库连接环境
void MainWindow::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");

    if (!db.open()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    // 确保表结构存在
    QString createTable = "CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY, val INTEGER)";
    if (!query.exec(createTable)) {
        qDebug() << "建表失败:" << query.lastError().text();
    }
}

// 分离功能：数据加载与界面同步
void MainWindow::loadData()
{
    QSqlQuery query("SELECT val FROM settings WHERE id = 1");

    if (query.next()) {
        Xuhao = query.value(0).toInt();
    } else {
        // 初始化默认数据
        QSqlQuery insert;
        insert.prepare("INSERT INTO settings (id, val) VALUES (1, ?)");
        insert.addBindValue(Xuhao);
        insert.exec();
    }
    updateUI();
}

// 分离功能：核心业务逻辑（自增）
void MainWindow::incrementXuhao()
{
    Xuhao++;
    updateUI();
    updateDatabase();
}

// 分离功能：纯粹的界面刷新
void MainWindow::updateUI()
{
    label->setText(QString("当前序号: %1").arg(Xuhao));
}

// 分离功能：数据持久化
void MainWindow::updateDatabase()
{
    QSqlQuery query;
    query.prepare("UPDATE settings SET val = ? WHERE id = 1");
    query.addBindValue(Xuhao);
    if (!query.exec()) {
        qDebug() << "同步数据库失败:" << query.lastError().text();
    }
}
