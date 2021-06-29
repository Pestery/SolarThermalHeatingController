import { ContollerViewModel } from './controller-view-model'

export function onNavigatingTo(args) {
  const component = args.object
  component.bindingContext = new ContollerViewModel()
}
