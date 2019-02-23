from django.db import models
from django.urls import reverse
from django.core.validators import MaxValueValidator, MinValueValidator
from django.contrib.auth.models import User
# Create your models here.

Status = (
    ('Open','Open'),
    ('Confirmed','Confirmed'),
    ('Completed','Completed')
)

class Ride(models.Model):
    # user = models.OneToOneField(User, on_delete=models.CASCADE)
    Destination = models.CharField(max_length=200)
    owner = models.ForeignKey(
        User,
        blank=True,
        null=True,
        related_name='owner_set',
        on_delete=models.CASCADE
    )
    sharer = models.CharField(max_length=200,default='')
    driver = models.CharField(max_length=200,default='')
    Arrival_Time = models.DateTimeField()
    Num_Passenger = models.IntegerField(validators=[MinValueValidator(1), MaxValueValidator(8)])
    Num_Sharer = models.IntegerField(default=0)

    Ride_status = models.CharField(max_length=10,choices=Status,default='Open')

#   The following two models are optional
    Vehicle_type = models.CharField(max_length=50,null=True, blank=True)
    Special_request = models.CharField(max_length=200,null=True, blank=True)

    def __str__(self):
        return f'{self.Destination},{self.Arrival_Time}'

    def get_absolute_url(self):
        return reverse('ride-detail', args=[str(self.id)])


class Share(models.Model):
    sharer = models.ForeignKey(
        User,
        on_delete=models.CASCADE,
        default=''
    )
    Destination = models.CharField(max_length=200)
    Arrival_Time_0 = models.DateTimeField(default='2018-02-05 12:00',verbose_name='Earliest Acceptable Arrival Time',help_text='Format: 2019-02-05 12:00')
    Arrival_Time_1 = models.DateTimeField(default='2019-02-05 12:00',verbose_name='latest Acceptable Arrival Time',help_text='Format: 2019-02-05 12:00')
    Num_Passenger = models.IntegerField(default=1)

    def __str__(self):
        return f'{self.Destination},{self.Arrival_Time}'

    def get_absolute_url(self):

        return reverse('share-list')


class Profile(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)
    is_driver= models.BooleanField(null=True, blank = True)
    license_plate_number = models.CharField(
                                max_length=30,
                                verbose_name="License Plate Number",
                                blank=True)
    Vehicle_Capacity = models.IntegerField(
                                    verbose_name="Maxmium Number of Passengers Allowed",
                                    default=0)
    Vehicle_Model = models.CharField(
                                max_length=30,
                                verbose_name="Vehicle_Model",
                                blank=True)

    def __str__(self):
        return f'{self.user.username} Profile'