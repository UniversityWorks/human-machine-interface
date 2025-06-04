#include "mainwindow.h"
#include <QToolTip>
#include <QMessageBox>
#include <QFont>
#include <QEnterEvent>
#include <QTimer>
#include <QStyle>


DeliveryButton::DeliveryButton(const QString &text, const QString &tooltip, QWidget *parent)
    : QPushButton(text, parent), m_tooltip(tooltip), m_isSelected(false)
{
    setupButton();
    setupTooltipTimer();
    setupAnimation();
}

void DeliveryButton::setSelected(bool selected)
{
    m_isSelected = selected;
    updateButtonStyle();
}

void DeliveryButton::enterEvent(QEnterEvent *event)
{
    QPushButton::enterEvent(event);
    setCursor(Qt::PointingHandCursor);


    setProperty("hovered", true);
    style()->unpolish(this);
    style()->polish(this);


    m_tooltipTimer->start(2000);
}

void DeliveryButton::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    setCursor(Qt::ArrowCursor);


    setProperty("hovered", false);
    style()->unpolish(this);
    style()->polish(this);

    m_tooltipTimer->stop();
    QToolTip::hideText();
}

void DeliveryButton::showTooltip()
{
    QPoint globalPos = mapToGlobal(rect().center());
    QToolTip::showText(globalPos, m_tooltip);
}

void DeliveryButton::setupButton()
{

    setMinimumSize(120, 60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMaximumWidth(180);

    setFont(QFont("Arial", 12, QFont::Bold));
    updateButtonStyle();
}
void DeliveryButton::setupTooltipTimer()
{
    m_tooltipTimer = new QTimer(this);
    m_tooltipTimer->setSingleShot(true);
    connect(m_tooltipTimer, &QTimer::timeout, this, &DeliveryButton::showTooltip);
}

void DeliveryButton::setupAnimation()
{

    m_scaleAnimation = new QPropertyAnimation();
    m_scaleAnimation->setDuration(200);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutCubic);


}

void DeliveryButton::updateButtonStyle()
{
    QString baseStyle =
        "QPushButton {"
        "   background-color: %1;"
        "   border: 2px solid #3498db;"
        "   border-radius: 10px;"
        "   color: %2;"
        "   padding: 10px;"
        "   font-weight: bold;"
        "   transition: all 0.2s ease;"
        "}"
        "QPushButton:hover, QPushButton[hovered=\"true\"] {"
        "   border-radius: 15px;"
        "   border: 3px solid #2980b9;"
        "   transform: scale(1.05);"
        "}";

    if (m_isSelected) {
        setStyleSheet(baseStyle.arg("#3498db", "white"));
    } else {
        setStyleSheet(baseStyle.arg("#ecf0f1", "#2c3e50") +
                      "QPushButton:!hover { opacity: 0.7; }");
    }
}


ConfirmButton::ConfirmButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent), m_isActive(false)
{
    setupButton();
    setupAnimation();
}

void ConfirmButton::setActive(bool active)
{
    m_isActive = active;
    setEnabled(active);
    updateButtonStyle();

    if (active) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ForbiddenCursor);
    }
}

void ConfirmButton::enterEvent(QEnterEvent *event)
{
    QPushButton::enterEvent(event);
    if (m_isActive) {
        setProperty("hovered", true);
        style()->unpolish(this);
        style()->polish(this);
    }
}

void ConfirmButton::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    if (m_isActive) {
        setProperty("hovered", false);
        style()->unpolish(this);
        style()->polish(this);
    }
}

void ConfirmButton::mousePressEvent(QMouseEvent *event)
{
    if (m_isActive) {
        QPushButton::mousePressEvent(event);

        setProperty("pressed", true);
        style()->unpolish(this);
        style()->polish(this);

        QTimer::singleShot(100, this, [this]() {
            setProperty("pressed", false);
            style()->unpolish(this);
            style()->polish(this);
        });
    }
}

void ConfirmButton::setupButton()
{

    setMinimumSize(150, 50);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMaximumSize(250, 50);

    setFont(QFont("Arial", 14, QFont::Bold));
    updateButtonStyle();
}
void ConfirmButton::setupAnimation()
{

    m_hoverAnimation = new QPropertyAnimation();
    m_hoverAnimation->setDuration(200);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
}



void ConfirmButton::updateButtonStyle()
{
    QString activeStyle =
        "QPushButton {"
        "   background-color: #27ae60;"
        "   border: none;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 12px;"
        "   transition: all 0.2s ease;"
        "}"
        "QPushButton:hover, QPushButton[hovered=\"true\"] {"
        "   background-color: #2ecc71;"
        "   border-radius: 15px;"
        "   transform: scale(1.05);"
        "}"
        "QPushButton[pressed=\"true\"] {"
        "   transform: scale(0.95);"
        "}";

    QString inactiveStyle =
        "QPushButton {"
        "   background-color: #95a5a6;"
        "   border: none;"
        "   border-radius: 10px;"
        "   color: #7f8c8d;"
        "   font-weight: bold;"
        "   padding: 12px;"
        "}";

    setStyleSheet(m_isActive ? activeStyle : inactiveStyle);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_selectedDelivery(nullptr)
{
    setupUI();
    connectSignals();
    updateConfirmButton();
}

MainWindow::~MainWindow()
{
}


void MainWindow::onDeliveryButtonClicked()
{
    DeliveryButton *clickedButton = qobject_cast<DeliveryButton*>(sender());
    if (!clickedButton) return;

    for (DeliveryButton *button : m_deliveryButtons) {
        button->setSelected(false);
    }

    clickedButton->setSelected(true);
    m_selectedDelivery = clickedButton;

    updateConfirmButton();
}

void MainWindow::onConfirmButtonClicked()
{
    if (m_selectedDelivery) {
        QString selectedText = m_selectedDelivery->text();

        QMessageBox msgBox;
        msgBox.setWindowTitle("Вибір доставки");
        msgBox.setText(QString("Ви обрали спосіб доставки: %1").arg(selectedText));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setStyleSheet(
            "QMessageBox {"
            "   background-color: #ecf0f1;"
            "   font-size: 14px;"
            "}"
            "QMessageBox QPushButton {"
            "   background-color: #3498db;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 5px;"
            "   padding: 8px 16px;"
            "   font-weight: bold;"
            "min-width:300 px;"
            "min-height:80px;"
            "}"
            );
        msgBox.exec();
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Оберіть спосіб доставки");

    setMinimumSize(500, 400);
    resize(500, 400);

    setStyleSheet("QMainWindow { background-color: #f8f9fa; }");


    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(50, 50, 50, 50);


    QLabel *titleLabel = new QLabel("Оберіть спосіб доставки:");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 18, QFont::Bold));
    titleLabel->setStyleSheet("color: #2c3e50; margin-bottom: 20px;");
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *deliveryLayout = new QHBoxLayout();
    deliveryLayout->setSpacing(20);

    m_courierButton = new DeliveryButton("🚚 Кур'єр", "Доставка кур'єром до дверей");
    m_postButton = new DeliveryButton("📦 Пошта", "Отримання у відділенні пошти");
    m_pickupButton = new DeliveryButton("🏬 Самовивіз", "Самостійне отримання в магазині");

    m_deliveryButtons = {m_courierButton, m_postButton, m_pickupButton};


    deliveryLayout->addStretch();
    deliveryLayout->addWidget(m_courierButton);
    deliveryLayout->addWidget(m_postButton);
    deliveryLayout->addWidget(m_pickupButton);
    deliveryLayout->addStretch();

    mainLayout->addLayout(deliveryLayout);

    mainLayout->addStretch();


    m_confirmButton = new ConfirmButton("Обрати доставку");
    QHBoxLayout *confirmLayout = new QHBoxLayout();
    confirmLayout->addStretch();
    confirmLayout->addWidget(m_confirmButton);
    confirmLayout->addStretch();

    mainLayout->addLayout(confirmLayout);
}
void MainWindow::connectSignals()
{
    for (DeliveryButton *button : m_deliveryButtons) {
        connect(button, &QPushButton::clicked, this, &MainWindow::onDeliveryButtonClicked);
    }

    connect(m_confirmButton, &QPushButton::clicked, this, &MainWindow::onConfirmButtonClicked);
}

void MainWindow::updateConfirmButton()
{
    bool hasSelection = (m_selectedDelivery != nullptr);
    m_confirmButton->setActive(hasSelection);
}

