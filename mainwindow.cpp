#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 按业务步骤调用，过程与功能完全分离
    setupUI();
    setupShortcuts();
    initDatabase();
    loadData();
}

MainWindow::~MainWindow()
{
    db.close();
}

// --- 第一部分：快捷键触发的业务流 ---
void MainWindow::onShortcutActivated()
{
    // A. 格式化时间 (yyyyMMdd-HHmmss)
    QString timeStr = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");

    // B. 组合字符串 (时间-序号)
    QString result = QString("%1-%2").arg(timeStr).arg(Xuhao);

    // C. 写入剪贴板 (AHK 的 clipboard := ...)
    QApplication::clipboard()->setText(result);

    // D. 序号自增并保存 (AHK 的 NextVal := CurrentVal + 1 ...)
    incrementAndSave();

    // E. 状态反馈 (代替 AHK 的 ToolTip)
    statusBar()->showMessage(QString("已生成并复制: %1").arg(result), 2000);
}

// --- 第二部分：功能模块实现 ---

void MainWindow::setupShortcuts()
{
    // 绑定快捷键 Ctrl+Shift+C
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+Q"), this);
    connect(shortcut, &QShortcut::activated, this, &MainWindow::onShortcutActivated);
}

void MainWindow::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("CounterData.db");

    if (!db.open()) {
        qDebug() << "DB Error:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    // 创建表 (对应 AHK 的 CREATE TABLE IF NOT EXISTS MyCounter)
    query.exec("CREATE TABLE IF NOT EXISTS MyCounter (id INTEGER PRIMARY KEY, val INTEGER);");
    // 初始化首行数据
    query.exec("INSERT INTO MyCounter (id, val) SELECT 1, 1 WHERE NOT EXISTS (SELECT 1 FROM MyCounter WHERE id=1);");
}

void MainWindow::loadData()
{
    QSqlQuery query("SELECT val FROM MyCounter WHERE id=1;");
    if (query.next()) {
        Xuhao = query.value(0).toInt();
    }
    updateLabel();
}

void MainWindow::incrementAndSave()
{
    Xuhao++;

    QSqlQuery query;
    query.prepare("UPDATE MyCounter SET val = ? WHERE id = 1;");
    query.addBindValue(Xuhao);
    query.exec();

    updateLabel();
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18px; font-family: 'Consolas'; color: #2c3e50;");

    layout->addWidget(label);
    setCentralWidget(centralWidget);
    setWindowTitle("序号管理器");
    resize(300, 150);
}

void MainWindow::updateLabel()
{
    label->setText(QString("待用序号: %1").arg(Xuhao));
}
