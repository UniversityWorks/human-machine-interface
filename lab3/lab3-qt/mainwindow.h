#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QTimer>
#include <QList>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class DeliveryButton : public QPushButton
{
    Q_OBJECT

public:
    DeliveryButton(const QString &text, const QString &tooltip, QWidget *parent = nullptr);
    void setSelected(bool selected);
    bool isSelected() const { return m_isSelected; }

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void showTooltip();

private:
    void setupButton();
    void setupTooltipTimer();
    void setupAnimation();
    void updateButtonStyle();

private:
    QString m_tooltip;
    bool m_isSelected;
    QTimer *m_tooltipTimer;
    QPropertyAnimation *m_scaleAnimation;
    QRect m_originalGeometry;
};

class ConfirmButton : public QPushButton
{
    Q_OBJECT

public:
    ConfirmButton(const QString &text, QWidget *parent = nullptr);
    void setActive(bool active);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void setupButton();
    void setupAnimation();
    void animatePress();
    void updateButtonStyle();

private:
    bool m_isActive;
    QPropertyAnimation *m_hoverAnimation;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDeliveryButtonClicked();
    void onConfirmButtonClicked();

private:
    void setupUI();
    void connectSignals();
    void updateConfirmButton();

private:
    QWidget *m_centralWidget;
    QList<DeliveryButton*> m_deliveryButtons;
    DeliveryButton *m_courierButton;
    DeliveryButton *m_postButton;
    DeliveryButton *m_pickupButton;
    ConfirmButton *m_confirmButton;
    DeliveryButton *m_selectedDelivery;
};

#endif // MAINWINDOW_H
