/****************************************************************************
 *
 *   (c) 2017 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "LifetimeChecker.h"
#include "AirMapSharedState.h"
#include "AirspaceFlightPlanProvider.h"

#include <QObject>
#include <QTimer>
#include <QList>
#include <QGeoCoordinate>

//-----------------------------------------------------------------------------
/// class to upload a flight
class AirMapFlightPlanManager : public AirspaceFlightPlanProvider, public LifetimeChecker
{
    Q_OBJECT
public:
    AirMapFlightPlanManager(AirMapSharedState& shared, QObject *parent = nullptr);

    PermitStatus    flightPermitStatus  () const override { return _flightPermitStatus; }
    void            createFlight        (MissionController* missionController) override;
    QString         flightID            () { return _flightPlan; }

signals:
    void            error               (const QString& what, const QString& airmapdMessage, const QString& airmapdDetails);

private slots:
    void _pollBriefing                  ();
    void _missionBoundingCubeChanged    ();

private:
    void _uploadFlightPlan              ();
    void _createFlightPlan              ();
    void _deleteFlightPlan              ();

private:
    enum class State {
        Idle,
        GetPilotID,
        FlightUpload,
        FlightPolling,
        FlightDelete
    };

    struct Flight {
        QList<QGeoCoordinate> coords;
        QGeoCoordinate  takeoffCoord;
        float maxAltitude = 0;
        void reset() {
            coords.clear();
            maxAltitude = 0;
        }
    };

    Flight                  _flight;                ///< flight pending to be uploaded
    State                   _state = State::Idle;
    AirMapSharedState&      _shared;
    QTimer                  _pollTimer;             ///< timer to poll for approval check
    QString                 _flightPlan;            ///< Current flight plan
    QString                 _pilotID;               ///< Pilot ID in the form "auth0|abc123"
    MissionController*      _controller = nullptr;
    bool                    _createPlan = true;

    AirspaceFlightPlanProvider::PermitStatus _flightPermitStatus = AirspaceFlightPlanProvider::PermitUnknown;

};
