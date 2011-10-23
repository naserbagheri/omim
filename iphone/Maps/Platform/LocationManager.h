#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <UIKit/UIApplication.h>

#include "../../platform/location.hpp"

@protocol LocationObserver
@required
  - (void)onLocationStatusChanged:(location::TLocationStatus)newStatus;
  - (void)onGpsUpdate:(location::GpsInfo const &)info;
  - (void)onCompassUpdate:(location::CompassInfo const &)info;
@end

@interface LocationManager : NSObject <CLLocationManagerDelegate>
{
  CLLocationManager * m_locationManager;
  BOOL m_isStarted;
  BOOL m_reportFirstUpdate;
  NSMutableSet * m_observers;
}

- (void)start:(id <LocationObserver>)observer;
- (void)stop:(id <LocationObserver>)observer;
- (CLLocation *)lastLocation;
// Fixes compass angle orientation when rotating screen to landscape
- (void)setOrientation:(UIInterfaceOrientation)orientation;
@end
