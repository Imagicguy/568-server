from django.urls import path, include
from . import views
from django.contrib.auth import views as auth_views
from django.contrib import admin

urlpatterns = [
    path('', views.index, name='index'),
    path('rides/', views.RideListView.as_view(), name='rides'),
    path('ride/<int:pk>', views.RideDetailView.as_view(), name='ride-detail'),

]


#Create/ Update/ Delete Rides
urlpatterns += [
    path('ride/create/', views.RideCreate.as_view(), name='ride_create'),
    path('ride/<int:pk>/update/', views.RideUpdate.as_view(), name='ride_update'),
    path('ride/<int:pk>/update_driver/', views.RideUpdate_driver.as_view(), name='ride_update_driver'),
    path('ride/<int:pk>/update_sharer/', views.RideUpdate_sharer.as_view(), name='ride_update_sharer'),
    path('ride/<int:pk>/delete/', views.RideDelete.as_view(), name='ride_delete'),
]

#Owner
urlpatterns+=[
    path('owner/<int:pk>/', views.OwnerDetailView.as_view(), name='owner_detail'),
]

#Share
urlpatterns +=[
    path('share/create/', views.ShareCreate.as_view(), name='share_create'),
    path('share/<int:pk>/', views.ShareDetailView.as_view(), name='share_detail'),
    path('share/list/', views.SharePickRideListView.as_view(), name='share-list'),
    path('sharer/myrides', views.SharerConfirmRideListView.as_view(), name='sharer_confirm'),
    path('sharer/myrides/<int:pk>/', views.SharerConfirmDetailView.as_view(), name='sharer_confirm_detail'),
    path('sharer/<int:ride_id>/join/', views.share_join, name='share-join'),
    path('sharer/<int:ride_id>/cancel/', views.share_cancel, name='share-cancel'),


]

#driver
urlpatterns +=[
    path('drive/<int:pk>/', views.DriverDetailView.as_view(), name='drive_detail'),
    path('driver/myrides/<int:pk>/', views.DriverConfirmDetailView.as_view(), name='driver_confirm_detail'),
    path('driver/driverlist', views.DriverRideListView.as_view(), name='driver_list'),
    path('driver/myrides', views.DriverConfirmRideListView.as_view(), name='driver_confirm'),
    path('driver/<int:ride_id>/confirm/', views.ride_confirm, name='ride_confirm'),
    path('driver/<int:ride_id>/complete/', views.ride_complete, name='ride_complete'),
]


# User
urlpatterns += [
    # path('admin/', admin.site.urls),
    path('signup/', views.register, name='signup'),
    path('profile', views.profile,name='profile'),
    # path('login/', auth_views.LoginView.as_view(template_name='registration/login.html'), name='login'),
    # path('logout/', auth_views.LogoutView.as_view(template_name='registration/logged_out.html'), name='logout'),
]

#Owner, Sharer and Driver
urlpatterns +=[
    path('ownerlist', views.OwnerRideListView.as_view(), name='owner_list'),
    path('driver', views.driver, name='driver'),
    path('sharer', views.sharer, name='sharer'),
    path('owner', views.owner, name='owner'),
]