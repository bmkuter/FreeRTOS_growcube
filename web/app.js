// This file is the entirety of the app Component
// Vue components have data and lifecycle management functions
// Interact with the components only through lifecycle functions, never through the DOM e.g. getElementById and element.addClass("myClass")

import PodComponent from './pod.js'

export default {
    components: { PodComponent },
    // template is the HTML part of the component. @submit.prevent="submit" does just what you think, prevents default submit. @click is a vue wrapper for onclick and submit is the name of the function being called on click
    template: `
    <pod-component :settings="this.data"></pod-component>
    `
}