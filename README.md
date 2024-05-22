# UE Significance Extensions

This plugin contains:

* a component `USEControllerSignificanceUpdaterComponent` that you can use to call the `Updte` function of the `SignificanceManager` if you don't want to (or cannot) do that from for example the client viewport class.
* a component `USEGetSignificanceComponent` that will register to the `SignificanceManager` and compute the significance of the actor.
* an interface `USEGetSignificanceInterface` you can implement on your actors to allow them to override the functions of the component.

## Significance computation

1. If `bUseFixedSignificance` is set, then the significance is given by the property `FixedSignificance`
2. If the owner of the component implemements the interface `USEGetSignificanceInterface` then the significance is the result of the function `GetSignificance` calld on the actor
3. If the component is a blueprint and the function `K2_GetSignificance` has been implemented, then the significance is the result of this function
4. Else the significance is computed from the `SignificanceDistances` array 