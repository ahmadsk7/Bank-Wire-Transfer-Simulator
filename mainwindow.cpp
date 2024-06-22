#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bank.h" // Include your backend header

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_transferButton_clicked()
{
    // Get account details and amount
    std::string accountFrom = ui->accountFrom->text().toStdString();
    std::string accountTo = ui->accountTo->text().toStdString();
    double amount = ui->amount->text().toDouble();

    // Call the backend function for wire transfer
    bool success = transferMoney(accountFrom, accountTo, amount);

    if (success) {
        ui->resultLabel->setText("Transfer successful!");
    } else {
        ui->resultLabel->setText("Transfer failed!");
    }
}
