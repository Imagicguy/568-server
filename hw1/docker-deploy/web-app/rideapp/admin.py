from django.contrib import admin

# Register your models here.
from rideapp.models import Ride, Profile, Share
admin.site.register(Profile)

class RideAdmin(admin.ModelAdmin):
    list_display = ('Destination', 'Arrival_Time', 'Num_Passenger', 'Vehicle_type', 'Special_request')


admin.site.register(Ride, RideAdmin)

class ShareAdmin(admin.ModelAdmin):
    list_display = ('Destination', 'Arrival_Time_0','Arrival_Time_1', 'Num_Passenger')

admin.site.register(Share,ShareAdmin)