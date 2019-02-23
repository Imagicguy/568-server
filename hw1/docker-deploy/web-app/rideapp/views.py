from django.shortcuts import render, redirect, get_object_or_404
from rideapp.models import Ride, Share
from django.views import generic
from django.contrib.auth.mixins import LoginRequiredMixin,UserPassesTestMixin
from django.contrib.auth.decorators import login_required
from .forms import UserRegisterForm
from django.contrib import messages
from .forms import UserRegisterForm, UserUpdateForm, ProfileUpdateForm
from django.core.mail import send_mail

# Create your views here.
def index(request):
    num_ride = Ride.objects.all().count()
    context = {
        'num_ride' : num_ride,
    }

    return render(request, 'index.html', context = context)



@login_required
def sharer(request):
    num_ride = Ride.objects.all().exclude(sharer=request.user).count()
    context = {
        'num_ride' : num_ride,
    }
    return render(request, 'sharer.html', context = context)

@login_required
def driver(request):
    num_ride = Ride.objects.filter(Ride_status="Open",
                                   Num_Passenger__lte=request.user.profile.Vehicle_Capacity,
                                   ).exclude(owner=request.user).exclude(sharer=request.user).count()
    context = {
        'num_ride' : num_ride,
    }
    return render(request, 'driver.html', context = context)

@login_required
def owner(request):
    return render(request, 'owner.html')

@login_required
def share_join(request, ride_id):
    ride = Ride.objects.filter(pk=ride_id).first()
    ride.sharer = request.user.username
    share = Share.objects.all().last()
    ride.Num_Sharer=share.Num_Passenger
    ride.Num_Passenger=ride.Num_Passenger+ride.Num_Sharer
    ride.save()

    return redirect('../../myrides')

@login_required
def share_cancel(request, ride_id):
    ride = Ride.objects.filter(pk=ride_id).first()
    ride.Num_Passenger=ride.Num_Passenger-ride.Num_Sharer
    ride.Num_Sharer = 0
    ride.sharer = ''
    ride.save()
    return redirect('../../myrides')

@login_required
def ride_confirm(request, ride_id):
    ride = Ride.objects.filter(pk=ride_id).first()
    ride.driver = request.user.username
    ride.Ride_status="Confirmed"
    ride.save()
    send_mail(
        'Ride Confirmed',
        'Your Ride has been confirmed successfully.',
        'Cber Team',
        [request.user.email],
        fail_silently=False,
    )
    return redirect('../../myrides')

@login_required
def ride_complete(request, ride_id):
    ride = Ride.objects.filter(pk=ride_id).first()

    ride.Ride_status = "Completed"
    ride.save()
    return redirect('../../myrides')



class RideListView(LoginRequiredMixin,generic.ListView):
    model = Ride
    def get_queryset(self):
        return Ride.objects.filter(Ride_status="Open")

    paginate_by = 10

class RideDetailView(LoginRequiredMixin,generic.DetailView):
    model = Ride

from django.views.generic.edit import CreateView, UpdateView, DeleteView
from django.urls import reverse_lazy

class OwnerRideListView(LoginRequiredMixin,generic.ListView):
    template_name = 'rideapp/ride_list_owner.html'
    def get_queryset(self):
        return Ride.objects.filter(owner=self.request.user).exclude(Ride_status="Completed")

class DriverRideListView(LoginRequiredMixin,generic.ListView):
    template_name = 'rideapp/ride_list_driver.html'


    def get_queryset(self):
        return Ride.objects.filter(Ride_status="Open",
                                   Num_Passenger__lte=self.request.user.profile.Vehicle_Capacity,
                                   ).exclude(owner=self.request.user).exclude(sharer=self.request.user)

    # def test_func(self):
    #     if self.request.user.profile.is_driver == True :
    #         return True
    #     return False

class DriverConfirmRideListView(LoginRequiredMixin,generic.ListView):
    template_name = 'rideapp/ride_list_driver_confirm.html'
    def get_queryset(self):
        return Ride.objects.filter(driver=self.request.user,Ride_status="Confirmed")

class SharerConfirmRideListView(LoginRequiredMixin,generic.ListView):
    template_name = 'rideapp/ride_list_sharer_confirm.html'
    def get_queryset(self):
        return Ride.objects.filter(sharer=self.request.user).exclude(Ride_status="Completed")



class RideCreate(LoginRequiredMixin,CreateView):
    model = Ride
    fields =['Destination', 'Arrival_Time', 'Num_Passenger', 'Vehicle_type', 'Special_request']
    initial = {'Arrival_Time': '05/01/2018 12:00'}

    def form_valid(self, form):
        form.instance.owner = self.request.user
        return super().form_valid(form)



class RideUpdate(LoginRequiredMixin,UpdateView):
    model = Ride
    fields = ['Destination', 'Arrival_Time', 'Num_Passenger', 'Vehicle_type', 'Special_request']

class RideUpdate_driver(LoginRequiredMixin,UpdateView):
    model = Ride
    fields = ['Ride_status']

class RideUpdate_sharer(LoginRequiredMixin,UpdateView):
    model = Ride
    fields = ['Special_request']
    # def form_valid(self, form):
    #     form.instance.sharer = self.request.user
    #     return super().form_valid(form)


class RideDelete(DeleteView):
    model = Ride
    success_url = reverse_lazy('rides')

class ShareCreate(LoginRequiredMixin, CreateView):
    model = Share
    fields = ['Destination','Arrival_Time_0','Arrival_Time_1','Num_Passenger']


    def form_valid(self, form):
        form.instance.sharer = self.request.user
        return super().form_valid(form)


class ShareRideListView(LoginRequiredMixin,generic.ListView):
    def get_queryset(self):
        return Share.objects.all()


class SharePickRideListView(LoginRequiredMixin,generic.ListView):
    template_name = 'rideapp/share_list.html'
    def get_queryset(self):
        share = self.request.user.share_set.last()
        return Ride.objects.filter(
                                   Destination=share.Destination,
                                    Arrival_Time__gte=share.Arrival_Time_0,
                                    Arrival_Time__lte=share.Arrival_Time_1,
                                   Ride_status="Open"
                                   ).exclude(owner=self.request.user).exclude(driver=self.request.user).exclude(sharer=self.request.user)



class ShareDetailView(LoginRequiredMixin,generic.DetailView):
    template_name = 'rideapp/ride_share_detail.html'
    model = Ride

class SharerConfirmDetailView(LoginRequiredMixin,generic.DetailView):
    template_name = 'rideapp/share_detail_confirm.html'
    model = Ride



class DriverDetailView(LoginRequiredMixin,generic.DetailView):
    template_name = 'rideapp/ride_drive_detail.html'
    model = Ride

class DriverConfirmDetailView(LoginRequiredMixin, generic.DetailView):
    template_name = 'rideapp/drive_detail_confirm.html'
    model = Ride


class OwnerDetailView(LoginRequiredMixin,generic.DetailView):
    template_name = 'rideapp/ride_own_detail.html'
    model = Ride


def register(request):
    if request.method == 'POST':
        form = UserRegisterForm(request.POST)
        if form.is_valid():
            form.save()
            username = form.cleaned_data.get('username')
            email = form.cleaned_data.get('email')
            messages.success(request, f'Account created for {username}!')
            send_mail(
                'Register Success',
                'Your account has been created successfully.',
                'Cber Team',
                [email],
                fail_silently=False,
            )
            return redirect('login')

    else:
        form = UserRegisterForm()

    return render(request, 'registration/signup.html', {'form': form})






@login_required
def profile(request):
    if request.method == 'POST':
        u_form = UserUpdateForm(request.POST, instance=request.user)
        p_form = ProfileUpdateForm(request.POST,
                                   request.FILES,
                                   instance=request.user.profile)
        if u_form.is_valid() and p_form.is_valid():
            u_form.save()
            p_form.save()
            messages.success(request, f'Your account has been updated!')
            return redirect('profile')

    else:
        u_form = UserUpdateForm(instance=request.user)
        p_form = ProfileUpdateForm(instance=request.user.profile)

    context = {
        'u_form': u_form,
        'p_form': p_form
    }

    return render(request, 'registration/profile.html', context)
