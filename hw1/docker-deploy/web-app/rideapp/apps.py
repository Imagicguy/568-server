from django.apps import AppConfig


class RideappConfig(AppConfig):
    name = 'rideapp'
    
    
    def ready(self):
        import rideapp.signals

